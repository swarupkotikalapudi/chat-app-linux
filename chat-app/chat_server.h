#ifndef _CHAT_SERVER_H
#define _CHAT_SERVER_H

#ifndef _CHAT_APP_H
  #include "chat_app.h"
#endif
#include<memory>

#include<iosfwd>


class cMsgQueue;
class cSemaphore;



class chatServer:public cChatApp
{
  
  chatServer(const chatServer&);
  chatServer& operator=(const chatServer&);
  
  void thread_handler1(); /* thread1 handles actions like print stats, restart server etc.. */
  void thread_handler2(); /* thread1 handles actions like wait for client connections, etc.. */
  
  
  
  /* Each thread's send and receive message queue pointer declared here.. */
  std::unique_ptr<cMsgQueue> pMsgQ_thrd1_S;
  std::unique_ptr<cMsgQueue> pMsgQ_thrd1_R;
  
  std::unique_ptr<cMsgQueue> pMsgQ_thrd2_S;
  std::unique_ptr<cMsgQueue> pMsgQ_thrd2_R;
  
  /* This semaphore is used to condition waiting between two threads e.g. one thread put a command using message queue in another thread. This
     is asynchronous behavior, however first thread need the result from second thread. So in this scenario first thread after putting message
     in a message queue will wait on this semaphore, the second thread after processing the command will do the semaphore post, which will
     unblock the first thread and it can resume execution of code... */
  std::unique_ptr<cSemaphore> m_pSemaphore;
  
protected:

public:
  chatServer(std::shared_ptr<cCmdLineParser> pCmdLineParser);
  ~chatServer();
  
  int   chatAppInit(int ListenQueueLength);
  
  int   startChatApp();
  
  int   stopChatApp();
  
  int   resourceClose_ChatApp();
  
  void  thread_handler(int iThreadIndex);
  
  int   printConnectionStats();
  void  printHelpOptions();
  
  void  sendPacket(const  char *, const int bufLen);
  
  void  recvdPacket(std::stringstream& sStream);
  
  
  
  int   attachCmdObject(std::shared_ptr<IChatEvtCmd> pEvtCmd);
   
  void   onFatalErrorOccured(int errNo);

  void  onClientConnectionClosed(int iPid);  
    
};

#endif /* _CHAT_SERVER_H */

