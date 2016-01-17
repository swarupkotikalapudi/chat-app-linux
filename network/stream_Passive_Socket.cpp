#ifndef _STREAM_PASSIVE_SOCKET_H
#include "stream_Passive_Socket.h"
#endif
#ifndef _ISOCKET_ADDRESS_H
#include "ISocket_Address.h"
#endif
#ifndef ADAPTED_SOCKET_ADDRESS_H
  #include "adapated_socket_address.h"
#endif

#ifndef _CSHAREDMEM_H
#include "cSharedMem.h"
#endif

#ifndef _C_CLOGGER_H
#include "cLogger.h"
#endif


#include<sys/socket.h>

#include<sys/un.h> /* unix domain socket */
#include <errno.h> //errno after a system call

#include <unistd.h>

streamPassiveSocket::streamPassiveSocket(int qLength):client_pend_q_Length(qLength)
{
  m_eConnectionType = socketCommunication_namespace::eSockServer;
  P_setThreadSupportNeeded(2);
}



streamPassiveSocket::~streamPassiveSocket(void)
{  
}



int streamPassiveSocket::Socket_Init(void)
{
  adapted_socket_address* pAdaptedSockAddr;
  int iNetworkDomain, iSocktype,iProtocol, optval,sfd = -1;
  typedef std::vector<struct sockAddrContainer>::const_iterator Iter;
  struct sockAddrContainer tContainer;
  bool bIsBinded = false;
  errno   = 0; /* Just clear the error no to start this function from clean state */
  
  if(! m_pSocketAddress || ! m_pSocketAddress.get() ) {
    errno = ENOMEM;
    return -1;
  }
  
  pAdaptedSockAddr  =   m_pSocketAddress->getAdaptedSocketAddress();
  if(!pAdaptedSockAddr) 
    return -1;
  
  iNetworkDomain    =   pAdaptedSockAddr->get_network_domain();
  iSocktype         =   pAdaptedSockAddr->get_sock_type();
  iProtocol         =   pAdaptedSockAddr->get_sock_protocol();   
  
  for (Iter it = pAdaptedSockAddr->get_sock_address().begin(); it != pAdaptedSockAddr->get_sock_address().end(); ++it) 
  {   
    sfd = socket(iNetworkDomain, iSocktype, iProtocol);
    if (sfd == -1)
      continue;   /* On error, try next address */
    
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,sizeof(optval)) == -1) {       
      if(sfd)   close(sfd);
        return -1;
    }
    
    /* Turn off Nagle's algorithm */
    //optval = 1;
    //setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(int)); //todo required cooment for testing

    tContainer = *it;
    
    if( iNetworkDomain == AF_UNIX )  {
      struct sockaddr_un *unixsockAddr = (struct sockaddr_un*) (tContainer.m_sockaddr);            
      unlink(unixsockAddr->sun_path);
      errno = 0;
    }
    
      /*bind() function in sys/sockets.h has name conflict with std::bind, hence it gives compilation error
      to avoid compilation error below bind() function is used as ::bind */
    if (::bind(sfd, (struct sockaddr *) tContainer.m_sockaddr, tContainer.sock_addrlen) == 0) { 
      bIsBinded = true; 
      break;  /* Success */
    }
    
    close(sfd); /* bind() failed: close this socket and try next address */
  } /* For loop ends here */
  
  if (sfd == -1)  { /*somehow no socket descriptor could be allocated, hence returning 'no memory available' error  */
    errno = ENOMEM;
    return -1; 
  }
  
  if(bIsBinded == false)  { /* somehow no bind succeed, hence return failure */    
    /*Note : As bind failed no need to call close() on opened socket.. */
    return -1;
  }
  
  if (listen(sfd, client_pend_q_Length) == -1)  {
    if(sfd)   close(sfd);
      return -1;
  }
  
  sock_fd = sfd;
  
  /*Create the shared memory region, which will be use by different client child process and parent process */ 
  m_psharedMem =   std::unique_ptr<cSharedMem>(new cSharedMem(socketCommunication_namespace::MAX_CLIENT_CONNECTION, "/SOCKET_SHARED_MEM_SERVER") );
  m_psharedMem->sharedMem_Open();
  
  m_pLogger = cLogger::getLogger();
  
  return 0;
  
}



void  streamPassiveSocket::Socket_printConnectionStats()
{
  printServerConnectionInfo();
}


