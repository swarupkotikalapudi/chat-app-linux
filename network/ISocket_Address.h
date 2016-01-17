#ifndef _ISOCKET_ADDRESS_H
#define _ISOCKET_ADDRESS_H

#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif
#include <memory>




class adapted_socket_address;

/*
  Purpose of this class:
  
  Socket communication need to specify network addess e.g. IP address , port no etc..
  The network layer could be many, but here we consider three classes
  a) AF_UNIX for local communication within the linux or unix box, basically single computer based communication
  b) AF_INET for IP4 based network
  c) AF_INET6 for IP6 based network

  Please not there could be different permutation of socket communication can happen e.g. 
  a)client is on IP4 , server is in IP6
  b) client is in IP6 and server is in Ip4
  c) client and server both on IP4
  d) client and server both on IP6
  e) Not scalable but useful for within linux box is unix local ip

  So this class ISocketAddress works as abstract base class, for different type of network address 
*/

class ISocketAddress
{
private:

  ISocketAddress(const ISocketAddress&);
  ISocketAddress& operator=(const ISocketAddress&);

  void delteUnixSockAddress();

protected:
  adapted_socket_address* m_sock_adapt_addr;
  
public:

  virtual adapted_socket_address* getAdaptedSocketAddress(void)=0;
  virtual int RegisterSocketAddress(const char* hostName, const char* service,int type, int domain, commondef_namespace::eConnectionType iConnType) = 0;
  virtual int     unRegisterSocketAddress(void) = 0;

  /*constructor */
  ISocketAddress();

  /*destructor */
  virtual   ~ISocketAddress(void);

};


#endif /* _ISOCKET_ADDRESS_H */

