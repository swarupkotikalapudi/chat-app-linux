#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif
#ifndef _CCHATEVTCOMMAND_H
  #include "cChatEvtCmd.h"
#endif

#ifndef _CHAT_APP_H
  #include "chat_app.h"
#endif

#include<sstream>

/*
  This class reprsents a command interface which can be used to send a command to client or server...
*/

cChatEvtCmd::evtCmdFuncPtrTable cChatEvtCmd::evtCmdTable[chatevtcmd_namespace::CMD_TABLE_ENTRIES] = {
   &cChatEvtCmd::onStartApp ,
   &cChatEvtCmd::onStopApp ,
   &cChatEvtCmd::onRestart ,
   &cChatEvtCmd::onInformSignal ,
   &cChatEvtCmd::onPrintStats ,
   &cChatEvtCmd::onHelpOptions ,
   &cChatEvtCmd::onNewClientCreated  ,
   &cChatEvtCmd::onChildRemoved  ,
   &cChatEvtCmd:: onChildMaxReached,
   &cChatEvtCmd:: onPacketSend,
   &cChatEvtCmd:: onResourceClose,
   &cChatEvtCmd:: onFatalError
};



cChatEvtCmd::cChatEvtCmd(std::shared_ptr<cChatApp> pChatApp):m_pChatApp(pChatApp)
{
  if( ! m_pChatApp ) {    
    throw "chatApp not initialize..";
  } 
   
}



cChatEvtCmd::~cChatEvtCmd()
{
}



int cChatEvtCmd::executeChatEvtCmd(chatevtcmd_namespace::eChatCommand chatCmd, void *ptr,  int iMsgVal)
{
  if(!m_pChatApp)
    return -1;
    
  return (this->*evtCmdTable[chatCmd])(ptr, iMsgVal);
}



int cChatEvtCmd::onStartApp(void *ptr, int iMsgVal)
{
  int iResult = 0;
  
  if( ( iResult = m_pChatApp->chatAppInit() ) != commondef_namespace::eSUCCESS) {
    return iResult;
  }
 
  if( ( iResult = m_pChatApp->startChatApp() )  != commondef_namespace::eSUCCESS) {
    return iResult;
  }
  return iResult;
}



int cChatEvtCmd::onStopApp(void *ptr, int iMsgVal)
{  
  return m_pChatApp->stopChatApp();
}



int cChatEvtCmd::onResourceClose(void *ptr, int iMsgVal)
{
  
  return m_pChatApp->resourceClose_ChatApp();
}



int cChatEvtCmd::onRestart(void *ptr, int iMsgVal)
{
  
  return -1;
}


 
int cChatEvtCmd::onInformSignal(void *ptr, int sig)
{
  m_pChatApp->informSignalListner(sig);
  return 0;
}



int cChatEvtCmd::onPrintStats(void *ptr, int iMsgVal)
{
  return  m_pChatApp->printConnectionStats();
}



int cChatEvtCmd::onHelpOptions(void *ptr, int iMsgVal)
{
  m_pChatApp->printHelpOptions();
  return 0;
}



int cChatEvtCmd::onNewClientCreated(void *ptr, int iMsgVal)
{
 
     
  return -1;
}

int cChatEvtCmd::onChildRemoved(void *ptr, int iMsgVal)
{
  m_pChatApp->onClientConnectionClosed(iMsgVal);
  return 0;
}
 
 
 
int cChatEvtCmd::onChildMaxReached(void *ptr, int iMsgVal)
{
  
  return -1;
}



int cChatEvtCmd::onPacketSend(void *ptr, int iMsgVal)
{      
  char* buf = (char*) ptr;
  if(buf) {
    m_pChatApp->sendPacket(buf, iMsgVal); /* buffer and buffer length is passed */
    return  0;
  }
  
  return -1;
}



int cChatEvtCmd::onFatalError(void *ptr, int iMsgVal)
{
  m_pChatApp->onFatalErrorOccured(iMsgVal);
  return 0;
}


int cChatEvtCmd::onStreamRecvd(std::stringstream& sStream)
{
  m_pChatApp->recvdPacket(sStream);
  
  return  0;
}


