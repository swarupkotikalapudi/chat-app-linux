#ifndef _IP_ADDRESS_H
#define _IP_ADDRESS_H

#ifndef _ISOCKET_ADDRESS_H
  #include "ISocket_Address.h"
#endif

#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif

namespace ipaddress_namespace
{
  const int  IP_ADDRESS_RULES  = 3;
}


class adapted_socket_address;

/* This class encapsulate IPv4/Ipv6 network domain related information */
class ipAddress:public ISocketAddress
{

private:
  struct socket_address_rules {
  int opt_sock_type;
  int iConnType;
  int ai_flags;
};
  
  
  int fillAddressInfo(int domain, int i_sockType, commondef_namespace::eConnectionType iConnType, struct addrinfo* hints);
  
  
  static socket_address_rules sock_addr_rules[ipaddress_namespace::IP_ADDRESS_RULES];
  
  ipAddress(const ipAddress&);
  ipAddress& operator=(const ipAddress&);


protected:


public:
 
  ~ipAddress(void);
   ipAddress();
  
  int RegisterSocketAddress(const char* hostName, const char* service,int i_sockType, int domain, commondef_namespace::eConnectionType iConnType);
  int unRegisterSocketAddress(void);
  
  adapted_socket_address* getAdaptedSocketAddress(void);
};



#endif /* _IP_ADDRESS_H */

