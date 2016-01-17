#ifndef _CCHATEVTCOMMAND_H
#define _CCHATEVTCOMMAND_H

#ifndef _ICHATEVTCMD_H
#include "IChatEvtCmd.h"
#endif

#include<memory>

class cChatEvtCmd;
class cChatApp;




class cChatEvtCmd : public IChatEvtCmd
{  
  typedef int (cChatEvtCmd::*evtCmdFuncPtrTable)(void *, int);
  
  chatevtcmd_namespace::eChatCommand m_eChatCmd;
    
  static evtCmdFuncPtrTable evtCmdTable[chatevtcmd_namespace::CMD_TABLE_ENTRIES];
    
  cChatEvtCmd();
  cChatEvtCmd(const cChatEvtCmd&);
  cChatEvtCmd& operator=(const cChatEvtCmd&);
  
  std::shared_ptr<cChatApp> m_pChatApp;
  
  int onStartApp(void *ptr,         int iMsgVal);
  int onStopApp(void *ptr,          int iMsgVal);
  int onRestart(void *ptr,          int iMsgVal);
  int onInformSignal(void *ptr,     int iMsgVal);
  int onPrintStats(void *ptr,       int iMsgVal);
  int onHelpOptions(void *ptr,      int iMsgVal);
  int onNewClientCreated(void *ptr, int iMsgVal);
  int onChildRemoved(void *ptr,     int iMsgVal);
  int onChildMaxReached(void *ptr,  int iMsgVal);
  int onPacketSend(void *ptr,       int iMsgVal);
  int onResourceClose(void *ptr,    int iMsgVal);
  int onFatalError(void *ptr,    int iMsgVal);

    
protected:
  
  
public:
  cChatEvtCmd(std::shared_ptr<cChatApp> pChatApp);
  ~cChatEvtCmd();
  
  int executeChatEvtCmd(chatevtcmd_namespace::eChatCommand chatCmd, void *ptr   = 0, int iMsgVal = 0);
  
  int onStreamRecvd(std::stringstream& sStream);
  
    
};


#endif /* _CCHATEVTCOMMAND_H */

