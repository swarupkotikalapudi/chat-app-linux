#ifndef _SOCKET_FACTORY_H
  #include "socket_factory.h"
#endif
#ifndef _STREAM_PASSIVE_SOCKET_H
  #include "stream_Passive_Socket.h"
#endif
#ifndef _STREAM_ACTIVE_SOCKET_H
  #include "stream_Active_Socket.h"
#endif
#ifndef _IP_ADDRESS_H
  #include "ip_address.h"
#endif
#ifndef _UNIX_LOCAL_IP_H
  #include "unix_local_ip.h"
#endif
#ifndef _STREAM_SOCKET_H
  #include "streamSocket.h"
#endif
#include<sys/socket.h>
#include <errno.h> //errno after a system call


  /* this rule is checked against argument pass to  'init' function of this class */
  socketFactory::socket_factory_rules socketFactory::sock_factory_rules[factorry_namespace::FACTORY_RULES_COUNT] = {
    /*This rule for 'TCP stream' server IPv4 socket */
    {AF_INET,  SOCK_STREAM, commondef_namespace::eSERVER } ,
    
    /*This rule for 'TCP stream' client IPv4 socket */
    {AF_INET6, SOCK_STREAM, commondef_namespace::eSERVER } ,
    
    /*This rule for 'TCP stream' Server Unix domain socket */
    {AF_UNIX,  SOCK_STREAM, commondef_namespace::eSERVER },
    
    /*This rule for 'TCP stream' client Unix domain socket */
    {AF_INET,  SOCK_STREAM, commondef_namespace::eCLIENT } ,
    
    /*This rule for 'TCP stream' Server IPv6 socket */
    {AF_INET6, SOCK_STREAM, commondef_namespace::eCLIENT } ,
    
    /*This rule for 'TCP stream' Client IPv6 socket */
    {AF_UNIX,  SOCK_STREAM, commondef_namespace::eCLIENT } ,
    
    {-1, -1, commondef_namespace::eNONE }
  };



socketFactory::socketFactory():m_iRuleIndx(-1)
{
}



socketFactory::~socketFactory()
{
}



int socketFactory::factory_init(int iSockType, int iDomain, commondef_namespace::eConnectionType connectionType)
{
  int iIndx = 0;
  socket_factory_rules *pRules = sock_factory_rules;

  while(1)  {
    if( pRules->opt_sock_type == -1 ) {
      errno = commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
      return -1;
    }
      
    if(pRules->network_domanin == iDomain && pRules->opt_sock_type == iSockType && pRules->iConnectionType == connectionType ) {
      m_iRuleIndx = iIndx;
      break;
    }
    pRules++;
    iIndx++;
  }
    
  return 0;
}



void socketFactory::createSocketObjects(int iSockType, int iDomain, commondef_namespace::eConnectionType iConnType, int iListenQLen,
  ISocketAddress**       pSocketAddress, ISocketCommunication** pSocketCommunication, ISocket** pSocket)
{
  try
  {
    /*creating network domain object e.g. IP4, IP6 or unix domain.. */
    if(iDomain == AF_INET || iDomain == AF_INET6) {
      *pSocketAddress = new ipAddress();
    }
    else if(iDomain == AF_UNIX) {
      *pSocketAddress = new unixLocalIP();
    } 
    /*Note if any time later nedded RAW socket etc has to be created here.. */
   
  
    if(iSockType == SOCK_STREAM)  
    { /*creating transport protocol tcp  object */
      
      *pSocket = new streamSocket(iConnType); /* ISocket dervied object created here */
      if(iConnType == commondef_namespace::eCLIENT)
        *pSocketCommunication = new streamActiveSocket(); /* stream or TCP client */
      else if(iConnType == commondef_namespace::eSERVER) {
        *pSocketCommunication = new streamPassiveSocket(iListenQLen);  /* stream or TCP server */
      }
    }    
    /* Note: UDP datagram etc related object has to be created here.. */
    
  }
   catch(std::exception& e)  {
    if(*pSocketAddress) { 
      delete *pSocketAddress;
      *pSocketAddress = nullptr;
    }
    if(*pSocketCommunication) {
      delete *pSocketCommunication;
      *pSocketCommunication = nullptr;
    }
    if(*pSocket) {
      delete *pSocket;
      *pSocket = nullptr;
    }
    errno = ENOMEM;
  }
  
}



std::shared_ptr<ISocket> socketFactory::createSocket(const char* hostName, const char* service, int listenQueueLength)
{
  int iResult = 0;
  ISocket*              pSocket = nullptr;
  ISocketCommunication* pSocketCommunication = nullptr;
  ISocketAddress*       pSocketAddress = nullptr;
  socket_factory_rules *pRules = sock_factory_rules;
  
  errno = 0;
 
  if(m_iRuleIndx == -1) { /* Error index in rule table not initialized */
    errno = commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
    return nullptr;
  }
  
  pRules = pRules + m_iRuleIndx;
  
  createSocketObjects( pRules->opt_sock_type, pRules->network_domanin, pRules->iConnectionType, listenQueueLength , &pSocketAddress,  &pSocketCommunication, &pSocket);
  
  if(pSocket == nullptr) {
    return nullptr;
  }
  
  std::shared_ptr<ISocket> pSocket_shared(pSocket);
  std::unique_ptr<ISocketAddress>        t_pSocketAddress(pSocketAddress);
  
  iResult = t_pSocketAddress->RegisterSocketAddress(hostName,service, pRules->opt_sock_type, pRules->network_domanin, pRules->iConnectionType);
  if(iResult != 0)  {
    return nullptr;
  }
     
  std::unique_ptr<ISocketCommunication>  t_pSocketCommunication(pSocketCommunication);
  
  /* std::move is used to transfer ownership to containing class for ISocketAddress* and ISocketCommunication* pointer object  */
  pSocketCommunication->setSocketAddress(std::move(t_pSocketAddress));
  pSocket_shared->setSocketCommunication(std::move(t_pSocketCommunication));
 
  return pSocket_shared;
}

