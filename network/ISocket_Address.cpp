#ifndef _ISOCKET_ADDRESS_H
#include "ISocket_Address.h" /*this header file give access to 'socket address' e.g. IP4/IP6/Unix etc..  */
#endif

#ifndef ADAPTED_SOCKET_ADDRESS_H
  #include "adapated_socket_address.h"
#endif

#include<sys/un.h>

#include<iostream>



ISocketAddress::ISocketAddress()
{    
  m_sock_adapt_addr = new adapted_socket_address();
}



ISocketAddress::~ISocketAddress(void)
{ 
  if(m_sock_adapt_addr) {
    if( m_sock_adapt_addr->get_network_domain() == AF_UNIX ) {
      delteUnixSockAddress();
    }
    
    delete m_sock_adapt_addr;
    m_sock_adapt_addr = NULL;
  }
  
}



void ISocketAddress::delteUnixSockAddress()
{
  typedef std::vector<struct sockAddrContainer>::const_iterator Iter;
  
  for (Iter it = m_sock_adapt_addr->get_sock_address().begin(); it != m_sock_adapt_addr->get_sock_address().end(); ++it) {
    struct sockAddrContainer tContainer = *it;
    struct sockaddr_un *unixsockAddr = (struct sockaddr_un*) (tContainer.m_sockaddr);
    if(unixsockAddr)  {
      delete  unixsockAddr;
      unixsockAddr  = nullptr;                    
    }
  }
  
}

