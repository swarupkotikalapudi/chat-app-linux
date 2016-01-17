#ifndef _CHAT_APP_H
#include "chat_app.h"
#endif
#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif

#include<sys/socket.h>



  /*Note : Increment the magic number of below array's 'array size' by 1, whenever a new rule is added. */
   /* These rules are checked against passed argument to check validity of the argument.. */
    cChatApp::socket_rules cChatApp::sock_rules[chatapp_namespace::CHAT_APP_RULES_COUNT] = {
    {AF_INET, SOCK_STREAM,  chatapp_namespace::eSUPPORTED } ,
    {AF_INET, SOCK_DGRAM,   chatapp_namespace::eEXE_NOT_SUPPORTING } ,
    {AF_INET6, SOCK_STREAM, chatapp_namespace::eSUPPORTED } ,
    {AF_INET6, SOCK_DGRAM,  chatapp_namespace::eEXE_NOT_SUPPORTING } ,
    {AF_UNIX, SOCK_STREAM,  chatapp_namespace::eSUPPORTED } ,
    {AF_UNIX, SOCK_DGRAM,   chatapp_namespace::eEXE_NOT_SUPPORTING } ,
    /* todo remaining e.g. RAW socket , UDP etc if supported need to be added in this table */
  
    { -1, -1, chatapp_namespace::eSUPPORT_NOT_APPLICABLE }
   };



cChatApp::cChatApp(std::shared_ptr<cCmdLineParser> pCmdLineParser): P_Thread(), m_pCmdLineParser(pCmdLineParser),pSocket(nullptr), m_eChatErrCode(commondef_namespace::eSUCCESS)
{

}



cChatApp::~cChatApp()
{  
}



int cChatApp::registerLinuxSignalHandler(fptrSignalHandler sigHandler)
{
  return registerSignalHandler(sigHandler);
}



chatapp_namespace::eSupportStatus cChatApp::validateSocketArguments(int network_domain, int socket_type)
{
  socket_rules *pRules = sock_rules;
  
  while(pRules->opt_network_domain != -1)
  {
    if( ( pRules->opt_network_domain == network_domain ) && ( pRules->opt_sock_type == socket_type) )  
    {
      if  (pRules->iStatus == chatapp_namespace::eSUPPORTED)  {
        return chatapp_namespace::eSUPPORTED;
      }
    }
    
    pRules++;
  }
  
  return chatapp_namespace::eEXE_NOT_SUPPORTING;
}



void  cChatApp::onClientConnectionClosed(int iPid)
{

}



void  cChatApp::sendPacket(const  char *, const int bufLen)
{
  
  return;
}


