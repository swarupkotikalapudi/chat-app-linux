#ifndef _UNIX_LOCAL_IP_H
#define _UNIX_LOCAL_IP_H

#ifndef _ISOCKET_ADDRESS_H
  #include "ISocket_Address.h" /*this header file give access to 'socket address' e.g. IP4/IP6/Unix etc..  */
#endif

class adapted_socket_address;

/*
Unix domain socket allow communication between different processes on the same machine..

This class represent Unix domain socket's address representation, which is required for socket programming
*/

class unixLocalIP:public ISocketAddress
{

private:

  unixLocalIP(const unixLocalIP&);
  unixLocalIP& operator=(const unixLocalIP&);
protected:


public:

  unixLocalIP();
  ~unixLocalIP();
  
  adapted_socket_address* getAdaptedSocketAddress();
  
  int RegisterSocketAddress(const char* hostName, const char* service,int type, int domain, commondef_namespace::eConnectionType iConnType);
  int     unRegisterSocketAddress(void);
  

};


#endif /* _UNIX_LOCAL_IP_H */

