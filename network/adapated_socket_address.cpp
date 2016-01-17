#ifndef ADAPTED_SOCKET_ADDRESS_H
#include "adapated_socket_address.h"
#endif

 
adapted_socket_address::adapted_socket_address()
{
}



adapted_socket_address::~adapted_socket_address()
{
  m_sockAddress.clear();
}



void adapted_socket_address::set_sock_type(int  sock_type)
{
  m_sock_type = sock_type;
}



void adapted_socket_address::set_sock_protocol(int  sock_protocol)
{
  m_sock_protocol = sock_protocol;
}


void adapted_socket_address::set_network_domain(int network_domain)
{
  m_network_domain  = network_domain;
}



int adapted_socket_address::get_sock_type(void)
{
  return m_sock_type;
}



int adapted_socket_address::get_sock_protocol(void)
{
  return m_sock_protocol;
}



int adapted_socket_address::get_network_domain(void)
{
  return  m_network_domain;
}


void adapted_socket_address::set_sock_address(const struct sockaddr* pSockaddr, const socklen_t sock_addrlen)
{
  sockAddrContainer tAddrContainer;
  tAddrContainer.m_sockaddr   = (struct sockaddr *) pSockaddr;
  tAddrContainer.sock_addrlen = sock_addrlen;
  m_sockAddress.push_back(tAddrContainer);

}



std::vector<struct sockAddrContainer>&  adapted_socket_address::get_sock_address()
{
  return m_sockAddress;
}

