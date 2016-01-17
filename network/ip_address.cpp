#ifndef _IP_ADDRESS_H
  #include "ip_address.h"
#endif
#ifndef ADAPTED_SOCKET_ADDRESS_H
  #include "adapated_socket_address.h"
#endif

#include <netdb.h>
#include<string.h>
#include <errno.h> //errno after a system call

  /* This is the rule which is compared with passed argument, to know if IPAddress class supports it */
  ipAddress::socket_address_rules ipAddress::sock_addr_rules[ipaddress_namespace::IP_ADDRESS_RULES] = 
  {
    {SOCK_STREAM ,  commondef_namespace::eSERVER,   AI_PASSIVE | AI_NUMERICSERV } ,
    {SOCK_STREAM,  commondef_namespace::eCLIENT,   AI_NUMERICSERV} ,
    //{SOCK_DGRAM,  CLIENT | SERVER ,  AI_NUMERICSERV},
  
    {-1, commondef_namespace::eNONE,  0}
  };



ipAddress::ipAddress(void)
{
}



ipAddress::~ipAddress(void)
{
}



int ipAddress::fillAddressInfo(int domain, int i_sockType, commondef_namespace::eConnectionType iConnType, struct addrinfo* hints)
{
  socket_address_rules *pRules = sock_addr_rules;
  
  while(1)
  {
    if(pRules->opt_sock_type == -1)  {
      errno = EINVAL;
      return -1;
    }
    
    if( ( pRules->opt_sock_type == i_sockType ) && ( pRules->iConnType & iConnType) )  {
      memset(hints, 0, sizeof(struct addrinfo));
      hints->ai_family = AF_UNSPEC;  /* Allows IPv4 or IPv6 */  
      hints->ai_flags = pRules->ai_flags ;
      hints->ai_socktype = pRules->opt_sock_type;
        
      if(i_sockType == SOCK_STREAM)
        hints->ai_protocol = IPPROTO_TCP;
      else if(i_sockType == SOCK_DGRAM)
        hints->ai_protocol = IPPROTO_UDP;
      else
        hints->ai_protocol =  0;
        
      hints->ai_canonname = NULL;
      hints->ai_addr = NULL;
      hints->ai_next = NULL;
      return 0;
    }
    
    pRules++;
   }
  
  return 0;
}



/* This function initialize the ip address related information */
int ipAddress::RegisterSocketAddress(const char* hostName, const char* service,int i_sockType, int domain, commondef_namespace::eConnectionType iConnType)
{
  struct addrinfo hints, *rp, *pIPaddrInfo = nullptr;
  int iResult;
  const char *tHostName = hostName;
  
  errno = 0; /* setting the errno , just to be clean any previously set errno */
  
  if( fillAddressInfo(domain, i_sockType, iConnType, &hints) == -1) {
    return errno; /* failure */  
  }
  
  if(*hostName == 0 )  {
    if(iConnType == commondef_namespace::eCLIENT)  {
      //todo throw exception here.. as hostname not passed..  
    }
    else {
      tHostName = nullptr;
    }
  }
    
  while(1)  {
    iResult = getaddrinfo(tHostName, service, &hints, &pIPaddrInfo);
    if(iResult == EAI_AGAIN)
      continue;
    
    if (iResult != 0 || !pIPaddrInfo) {      
      errno = iResult; /* failure */
      return errno;
    }
    
    break; /* It reaches here means getaddrinfo() success, hence break/exit from while loop */
  }
  
  rp = pIPaddrInfo;
 
  m_sock_adapt_addr->set_sock_type(rp->ai_socktype);
  m_sock_adapt_addr->set_sock_protocol(rp->ai_protocol);
  m_sock_adapt_addr->set_network_domain(rp->ai_family);
    
  for (; rp != NULL; rp = rp->ai_next)  {
     
    m_sock_adapt_addr->set_sock_address((sockaddr *) rp->ai_addr, rp->ai_addrlen);
  }
  
  freeaddrinfo(pIPaddrInfo);
    
  return 0;
}



int ipAddress::unRegisterSocketAddress(void)
{
  /*todo add code to remove ip address information */
  return 0;
}



adapted_socket_address* ipAddress::getAdaptedSocketAddress(void)
{
  return  m_sock_adapt_addr; 
}


