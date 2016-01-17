#ifndef _CHAT_APP_H
#define _CHAT_APP_H

#ifndef _LINUX_SIGNAL_MANAGER_H
  #include "linux_signal_manager.h"
#endif
#ifndef _P_THREAD_H
  #include "P_Thread.h"
#endif

#include<iosfwd>
#include<memory>

/*
  This class reprsents a abstract interface from chat client or chat server instance can be created..
*/
namespace chatapp_namespace
{
  const int CHAT_APP_RULES_COUNT  = 7;
  
  /* Todo -- this constant need to be read from configuration, instead of hard code like this.. */
  const int STREAM_SOCKET_QUEUE_LENGTH = 2; 

  enum eSupportStatus
  {
    eSUPPORTED,         /* Chat server suuport this feature */
    eOS_NOT_SUPPORTING, /* OS itself doesn't support this feature combination  */
    eEXE_NOT_SUPPORTING, /* OS supports, however the chat server binary the feature is not implemented  */
    eSUPPORT_NOT_APPLICABLE /*Don't care or 'not applicable' condition */
  };
  
 } 



class ISocket;
class IChatEvtCmd; 
class cCmdLineParser;

class P_Thread;



class cChatApp : public P_Thread, public linuxSignalManager
{
  struct socket_rules {
    int opt_network_domain;
    int opt_sock_type;
    chatapp_namespace::eSupportStatus iStatus;
  };
  
  cChatApp(const cChatApp&);
  cChatApp& operator=(const cChatApp&);
  
  typedef void (*fptrSignalHandler)(int); 
  
protected:
  static socket_rules sock_rules[chatapp_namespace::CHAT_APP_RULES_COUNT]; 
  std::shared_ptr<cCmdLineParser> m_pCmdLineParser;
  std::shared_ptr<ISocket> pSocket;

  
  /* Socket programming uses system call, any error is set in errno (in linux). 
  as this appliaction/design using multithreaded, each thread has its own version of errno. To share the errno after system call in all the  threads  this variable is declared */
  int             m_eChatErrCode;

  chatapp_namespace::eSupportStatus  validateSocketArguments(int network_domain, int socket_type);
  
public:
  
  cChatApp(std::shared_ptr<cCmdLineParser> pCmdLineParser);
  virtual ~cChatApp();
  
  virtual int   printConnectionStats()  = 0;
  virtual int   chatAppInit(int ListenQueueLength = chatapp_namespace::STREAM_SOCKET_QUEUE_LENGTH) = 0;
  virtual int   startChatApp()          = 0;
  virtual int   stopChatApp()           = 0;
  virtual int   resourceClose_ChatApp() = 0;
  virtual void  printHelpOptions()      = 0;
  
  virtual void  sendPacket(const  char *, const int bufLen);
  
  
  virtual void  recvdPacket(std::stringstream& sStream) = 0;
  
  virtual int   attachCmdObject(std::shared_ptr<IChatEvtCmd> pEvtCmd)       = 0;
  
  virtual void   onFatalErrorOccured(int errNo) = 0;
  
  virtual void  onClientConnectionClosed(int iPid);
   
  int  registerLinuxSignalHandler(fptrSignalHandler sigHandler);
  
};



#endif /* _CHAT_APP_H */

