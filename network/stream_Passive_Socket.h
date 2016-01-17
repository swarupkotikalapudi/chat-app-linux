#ifndef _STREAM_PASSIVE_SOCKET_H
#define _STREAM_PASSIVE_SOCKET_H

#ifndef _ISOCKET_COMMUNICATION_H
  #include "ISocket_Communication.h"
#endif

/*This class encapsulate passive socket or server connection */
class streamPassiveSocket: public ISocketCommunication
{

/*
  This parameter configures in the OS(Linux kernel), how many pending connection request from client will be queued by OS.
  This is passed in system call listen()
  This argument is useful when socket is used as server..
  */
  int client_pend_q_Length;
  
  streamPassiveSocket(const streamPassiveSocket&);
  streamPassiveSocket& operator=(const streamPassiveSocket&);

  
protected:

public:

  streamPassiveSocket(int qLength = socketCommunication_namespace::MAX_CLIENT_CONNECTION);
  ~streamPassiveSocket(void);
  
  int   Socket_Init(void);
  void Socket_printConnectionStats();
  
};

#endif /* _STREAM_PASSIVE_SOCKET_H */


