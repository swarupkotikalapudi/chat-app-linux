#ifndef _STREAM_ACTIVE_SOCKET_H
#include "stream_Active_Socket.h"
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


#include<sys/socket.h>
#include <errno.h> //errno after a system call
#include <unistd.h>


streamActiveSocket::streamActiveSocket(void)
{
  m_eConnectionType = socketCommunication_namespace::eSockClient;
 
  P_setThreadSupportNeeded(2);
}



streamActiveSocket::~streamActiveSocket(void)
{ 
  
}


int streamActiveSocket::Socket_Init(void)
{
  adapted_socket_address* pAdaptedSockAddr;
  int iNetworkDomain, iSocktype,iProtocol,sfd = -1, iRet = -1;
  typedef std::vector<struct sockAddrContainer>::const_iterator Iter;
  struct sockAddrContainer tContainer;
  Iter it;
  errno = 0; /* Just clear the error no to start this function from clean state */
  
  if(! m_pSocketAddress || ! m_pSocketAddress.get() ) {
    errno = ENOMEM;    
    return -1;
  }
  
  pAdaptedSockAddr  =   m_pSocketAddress->getAdaptedSocketAddress();
  if(!pAdaptedSockAddr) 
    return -1;
  
  iNetworkDomain           =   pAdaptedSockAddr->get_network_domain();
  iSocktype         =   pAdaptedSockAddr->get_sock_type();
  iProtocol         =   pAdaptedSockAddr->get_sock_protocol(); 
  
  for (it = pAdaptedSockAddr->get_sock_address().begin(); it != pAdaptedSockAddr->get_sock_address().end(); ++it) {
    
    sfd = socket(iNetworkDomain, iSocktype, iProtocol);
    if (sfd == -1)
      continue;   /* On error, try next address */
    
    tContainer = *it;
    
    if (connect(sfd, (struct sockaddr *) tContainer.m_sockaddr, tContainer.sock_addrlen) == 0) {     
      iRet = 0;
      sock_fd = sfd;
      break;  /* Success */
    }
    
    /* Connect failed: close this socket and try next address */
    close(sfd);
  }
  
  if(iRet == 0) {
    
    threadStart(pthread_namespace::eDETACH);
    threadStart(pthread_namespace::eDETACH);
  
    m_bThreadCreated = true;
    
    m_psharedMem =   std::unique_ptr<cSharedMem>(new cSharedMem(1, "/SOCKET_SHARED_MEM_CLIENT") );
    m_psharedMem->sharedMem_Open(); /*Create the shared memory region, which will be use by client process about Rx/Tx bytes */
  }
    
  return iRet;

}


void  streamActiveSocket::Socket_printConnectionStats()
{
    /*To do -- ncessary client print stats can be printed.
      
       Even though not printed, similar to server rx/tx for different clients, client's  rx/tx packet count is captured, if requried can be   printed here..
     */ 
}

