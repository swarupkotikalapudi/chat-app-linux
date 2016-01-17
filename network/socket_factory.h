#ifndef _SOCKET_FACTORY_H
#define _SOCKET_FACTORY_H

#ifndef _ISOCKET_H
  #include "ISocket.h"
#endif


class socketFactory;
class ISocketAddress;
class ISocketCommunication;
class ISocket;


namespace factorry_namespace
{
  const int FACTORY_RULES_COUNT = 7;
}

/*this class created socket, network domain related class  */
class socketFactory
{
private:
  struct socket_factory_rules {
    int network_domanin;
    int opt_sock_type;
    commondef_namespace::eConnectionType iConnectionType;
  };
  
  static socket_factory_rules sock_factory_rules[factorry_namespace::FACTORY_RULES_COUNT];
  
  inline void createSocketObjects(int iSockType, int iDomain, commondef_namespace::eConnectionType connectionType, int listenQueueLength,
                                  ISocketAddress**  pSocketAddress, ISocketCommunication**  pSocketCommunication, ISocket** pSocket);
  
  socketFactory(const socketFactory&);
  socketFactory& operator=(const socketFactory&);
  
  int m_iRuleIndx;
  
protected:

public:
  socketFactory();
  ~socketFactory();
  
  int factory_init(int iSockType, int iDomain, commondef_namespace::eConnectionType connectionType);
  std::shared_ptr<ISocket> createSocket(const char* hostName, const char* service, int listenQueueLength);
  
};

#endif /* _SOCKET_FACTORY_H */

