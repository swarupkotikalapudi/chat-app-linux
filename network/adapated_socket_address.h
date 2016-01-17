#ifndef ADAPTED_SOCKET_ADDRESS_H
#define ADAPTED_SOCKET_ADDRESS_H
#include<vector>
#include<sys/types.h>
#include<sys/socket.h>

typedef struct sockAddrContainer
  {
    struct sockaddr* m_sockaddr;
    socklen_t sock_addrlen;
  }sockAddrContainer;

/* This class is used to abstract/adapt different network domain type e.g. ipv4/ipv6 or unix domain socket */  
class adapted_socket_address
{
 
  std::vector<struct sockAddrContainer> m_sockAddress;
  int m_sock_type;
  int m_sock_protocol;
  int m_network_domain;
  
  adapted_socket_address(const adapted_socket_address&);
  adapted_socket_address& operator=(const adapted_socket_address&);
  
protected:
      
public:
  adapted_socket_address();
  ~adapted_socket_address();
  
  void set_sock_type(int  sock_type);
  void set_sock_protocol(int  sock_protocol);
  void set_network_domain(int network_domain);
  void set_sock_address(const struct sockaddr* pSockaddr,const  socklen_t sock_addrlen);
  
  int get_sock_type(void);
  int get_sock_protocol(void);
  int get_network_domain(void);
  
  std::vector<struct sockAddrContainer>&  get_sock_address();
  
};

#endif /* ADAPTED_SOCKET_ADDRESS_H */

