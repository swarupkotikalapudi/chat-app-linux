#ifndef _ISOCKET_COMMUNICATION_H
#define _ISOCKET_COMMUNICATION_H
#include<sys/socket.h>
#include<map>
#include<list>
#include<memory>

#ifndef _P_THREAD_H
  #include "P_Thread.h"
#endif

#ifndef _ISOCKET_H
#include "ISocket.h"
#endif



class ISocketAddress;
class cMutexCondwait;
class cSemaphore;
class cSharedMem;
class IChatEvtCmd;
class IPacket;
class cLogger;


namespace socketCommunication_namespace
{
  static const int MAX_PACKET_SIZE = 256;
  static const int RECV_BUFFER_SIZE    =  (MAX_PACKET_SIZE * 512);

  enum eSockConnectionType  
  {
    eSockServer = 2,
    eSockClient = 4
  };

  /*Todo: these rules are currently hardcoded, need to change it to read from configuration files and then initialize it */
  const int MAX_CLIENT_CONNECTION = 2;


  typedef std::map<pid_t,int> childProcessMap;

  typedef std::list<std::shared_ptr<IPacket> > PacketList;
}



class ISocketCommunication : public P_Thread
{
  ssize_t         Socket_Write_helper(int sfd, const  void *buffer, size_t n);
  int             Socket_Close_helper(int sfd);
  int             Socket_Shutdown_helper(int sfd, int how = SHUT_WR);
 
  void            handleClientRequest();
  void            getTimestamp(char* strTimeStamp);
  
  int             Socket_ServerHeartbeatMsg();
  
  ISocketCommunication(const ISocketCommunication&);
  ISocketCommunication& operator=(const ISocketCommunication&);
  
protected:
  int             sock_fd;
  
  int             m_iCntClientConnections;
  
  /*The socket address could be of diferent type e.g. IP4, IP6, Unix network etc.., ISocketAddress encapsulate these address related details */
  std::unique_ptr<ISocketAddress> m_pSocketAddress;
  
  std::unique_ptr<cSemaphore> m_pSemaphore;
  std::unique_ptr<cSharedMem> m_psharedMem;
  std::unique_ptr<cMutexCondwait> m_pMutexReadCondwait;
  std::unique_ptr<cMutexCondwait> m_pMutexWriteCondwait;
  
  socketCommunication_namespace::childProcessMap m_childProcessMap;
   
  std::shared_ptr<IChatEvtCmd>  m_pChatEvtCmd;
    
  void            printServerConnectionInfo();
  
  socketCommunication_namespace::PacketList      m_OutboundPacket; /* outbound packet going out from server */
  socketCommunication_namespace::PacketList      m_InboundPacket;  /* inbound packet coming to server */
  
  bool            m_bReadyToRead_InBndPkts;      
  bool            m_bReadyToSend_OutBndPkts;
  
  int             m_sendOfs;
  int             m_recvOfs, m_recvBegin; /* tracking the read head of the buffer */
  char            m_recvBuf[socketCommunication_namespace::RECV_BUFFER_SIZE];
  
  socketCommunication_namespace::eSockConnectionType m_eConnectionType;
  
  int             m_bThreadCreated;
  
  cLogger*        m_pLogger;
  
public:
                  ISocketCommunication(void);
                 
  void            setSocketAddress(std::unique_ptr<ISocketAddress> pSocketAddress);
  
  virtual         ~ISocketCommunication(void);
  
  virtual int     Socket_Init(void) = 0;
  virtual void    Socket_printConnectionStats() = 0;
  
  int             Socket_Process_Clients(void);
  
  int             Socket_Close(void);
  int             Socket_Shutdown(void);
  
  int             appShutdown(void);
  
  int             Socket_Erase_Child_Process_Details(pid_t pid);

  int             Socket_attachCommandObject(std::shared_ptr<IChatEvtCmd> pEvtCmd);
  
  int             Socket_doPacketSend(const char *buf, const int bufLen);
  int             Socket_doPacketRecv();
  
  int             Socket_HandleOutboundPackets();
  int             Socket_ProcessInboundPackets();  
  int             Socket_ReceiveInboundPackets();
  
  void            thread_handler(int iThreadIndex);
};

#endif /* _ISOCKET_COMMUNICATION_H */

