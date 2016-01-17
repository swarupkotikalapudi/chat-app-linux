#ifndef _UNIX_LOCAL_IP_H
#include "unix_local_ip.h"
#endif
#ifndef ADAPTED_SOCKET_ADDRESS_H
  #include "adapated_socket_address.h"
#endif


#include<sys/un.h>


static const char *SOCKNAME = "/tmp/unixSock";


unixLocalIP::unixLocalIP()
{  
}



unixLocalIP::~unixLocalIP(void)
{
}




int unixLocalIP::RegisterSocketAddress(const char* hostName, const char* service,int i_sockType, int domain, commondef_namespace::eConnectionType iConnType)
{
  struct sockaddr_un *unixsockAddr = (struct sockaddr_un*) new sockaddr_un();

  m_sock_adapt_addr->set_sock_type(i_sockType);  
  m_sock_adapt_addr->set_sock_protocol(0);
  m_sock_adapt_addr->set_network_domain(AF_UNIX);
  memset(unixsockAddr, 0, sizeof(struct sockaddr_un));
  unixsockAddr->sun_family = AF_UNIX;
  
  /* todo -- domain path name can be easily crashed by all kind of wrong input, so validation of input is needed here. 
    Currently hardcode the path name  */          
  strncpy(unixsockAddr->sun_path, SOCKNAME, sizeof(unixsockAddr->sun_path) - 1);
  m_sock_adapt_addr->set_sock_address( (struct sockaddr*) unixsockAddr, sizeof(struct sockaddr));

  return 0;
}



int unixLocalIP::unRegisterSocketAddress(void)
{
  
    
  return 0;
}



adapted_socket_address* unixLocalIP::getAdaptedSocketAddress(void)
{
  return  m_sock_adapt_addr;
}

