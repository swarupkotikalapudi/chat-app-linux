#ifndef _ICHATEVTCMD_H
#define _ICHATEVTCMD_H

#include<iosfwd>

namespace chatevtcmd_namespace
{
  static const int CMD_TABLE_ENTRIES = 12; 
  
  enum eChatCommand
  {
    eStartApp         =   0,
    eStopApp          =   1,
    eRestartApp       =   2,
    eInformSignal     =   3,
    ePrintStats       =   4,
    eHelpOptions      =   5,    
    eNewClientCreated =   6,
    eChildRemoved     =   7,
    eChildMaxReached  =   8,
    ePacketSend       =   9,
    eResourceClose    =   10,
    eFatalError       =   11,
    eChatStateNone    =   12
  };
}

/* This class an interface to command send to client or server application */
class IChatEvtCmd
{

  IChatEvtCmd(const IChatEvtCmd&);
  IChatEvtCmd& operator=(const IChatEvtCmd&);
  
protected:
  
public:
  IChatEvtCmd();
  ~IChatEvtCmd();
  
  virtual int executeChatEvtCmd(chatevtcmd_namespace::eChatCommand chatCmd, void* ptr = 0, int iMsgVal = 0) = 0;
  
  virtual int onStreamRecvd(std::stringstream& sStream) = 0;
};


#endif /* _ICHATEVTCMD_H */
