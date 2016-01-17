#ifndef _ISOCKET_H
#define _ISOCKET_H

#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif
#ifndef _ISIGNAL_LISTNER_H
  #include "Isignal_Listner.h"
#endif
#include<memory>


class ISocketCommunication;
class IChatEvtCmd;

/*This class encapsulate at high level socket interface, by using strategy pattern actual client/server functionality is provided by ISocketCommunication class */
class ISocket : public IsignalListner
{
  pid_t m_iProcessID;
  
  ISocket(const ISocket&);
  ISocket& operator=(const ISocket&);

protected:

  /* It represent abstract type of socket communication e.g. socket(), bind(), listen(), accept() etc..
  We know that socket connection is broadly divided into active and passive connection
  so for each transport layer protocol e.g. tcp/udp/sctp , there will be corresponding concerte active and passive class
  for e.g. tcp has stream_Active_Socket and stream_Passive_Socket */
  std::unique_ptr<ISocketCommunication> m_pSocketCommunication;

  /*This flag is not as such protocol specific, it is just to indicate if socket is server or client */
  commondef_namespace::eConnectionType       m_iConnectionType;
  
public:

                        ISocket();
                        ISocket( commondef_namespace::eConnectionType iConnType = commondef_namespace::eNONE);
  virtual               ~ISocket();
  
  int                   appShutdown(void);
  int                   Socket_Erase_Child_Process_Details(pid_t pid);  
    
  int                   Socket_Init(void);
  int                   Socket_Process_Clients(void);
  
  void                  Socket_printConnectionStats(void);
  
  int                   Socket_doPacketSend(const char *buffer, const int bufLen);
  int                   Socket_doPacketRecv();
  
  int                   Socket_Close(void);
  int                   Socket_Shutdown(void);
 
  void                  setSocketCommunication(std::unique_ptr<ISocketCommunication> pSocketCommunication);
  
  void                  signalEventOccured(int sig_event);
  
  bool                  Socket_Status();
  
  int                   Socket_attachCommandObject(std::shared_ptr<IChatEvtCmd> pEvtCmd);
};

#endif /* _ISOCKET_H */

