#ifndef _STREAM_ACTIVE_SOCKET_H
#define _STREAM_ACTIVE_SOCKET_H

#ifndef _ISOCKET_COMMUNICATION_H
#include "ISocket_Communication.h"
#endif

/*This class encapsulate active socket or client connection */
class streamActiveSocket: public ISocketCommunication
{
  streamActiveSocket(const streamActiveSocket&);
  streamActiveSocket& operator=(const streamActiveSocket&);

protected:
  

public:

  streamActiveSocket();
  ~streamActiveSocket(void);
  
  int Socket_Init(void);
  void Socket_printConnectionStats();
};




#endif /* _STREAM_ACTIVE_SOCKET_H */
