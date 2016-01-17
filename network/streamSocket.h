#ifndef _STREAM_SOCKET_H
#define _STREAM_SOCKET_H

#ifndef _ISOCKET_H
#include "ISocket.h"
#endif


/*
Socket is 
normal socket communication happens on two category a)stream b)datagram

TCP and SCTP fall under stream protocol

TCP/SCTP/UDP are host to host protocol, which are  implemented inside the OS

Socket gives abstract control over OS's implementation of protocol like tcp/udp/sctp etc..

So these class gives or wraps the functionality of tcp socket

*/

class streamSocket:public ISocket 
{
private:

protected:

public:
  streamSocket(commondef_namespace::eConnectionType iConnType = commondef_namespace::eNONE);
  ~streamSocket(void);
  
};


#endif /* _STREAM_SOCKET_H */

