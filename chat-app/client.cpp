#ifndef _COMMON_DEF_H
#include "common_def.h"
#endif
#ifndef _CCHATEVTCOMMAND_H
  #include "cChatEvtCmd.h"
#endif
#ifndef _CHAT_CLIENT_H
  #include "chat_client.h"
#endif
#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif
#ifndef _SIGNAL_DEFINATION_H
#include "signal_defination.h"
#endif
#ifndef _C_CMDLINEPARSER_H
#include "cCmdLineParser.h"
#endif

#ifndef _SOCKET_HELPER_H
  #include "socket_helper.h"
#endif

#include<map>
#include <errno.h> //errno after a system call
#include<string.h>
#include<sys/wait.h>


std::shared_ptr<cCmdLineParser> pCmdLineParser;
std::shared_ptr<cChatApp>     pClient;
std::shared_ptr<IChatEvtCmd>     pChatEvtCmd;


namespace 
{
  using socketHelper::waitOnUserInput;
  
  const int SELECT_CLIENT_BUF_LEN = 256; /* telnet  allow 4096 bytes, however using it as 256 bytes for user input */
  
  std::map<int,std::string> strSignalMsg;
  
  /* this function prepares the error message , which need to be print in signal handler */
  void prepareSignalMsg(void)
  {
      /*Caution: Please add here only fatal error meesage from which recovery not possible hence application will be closed.
             Please don't add custom and non fatal error message here.. */
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_PROCESS_ABORT, "Process abort") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_MEMORY_ACCESS_ERROR, "Memory access error") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_ARITHEMATIC_ERROR, "Arithmetic error") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_ILLEGAL_INSTRUCTION, "Illegal instruction") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_PARENT_PROCESS_TERMINATE, "Parent process terminate") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_FORCEFUL_TERMINAL_CLOSE, "Forceful terminal close") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_FORCEFUL_PROCESS_CLOSE, "Forceful process close") );
    strSignalMsg.insert( std::make_pair(signal_namespace::SIG_INVALID_MEMORY_REFERENCE, "Invalid memory reference") );
  }
    
  /*This function is called when any signal is raised by OS or application  */  
  void signalOccured(int sig)
  {
    bool bFatalError = false;
    std::map<int, std::string>::iterator it;
    it = strSignalMsg.find(sig);
  
    if(it != strSignalMsg.end())  {
      std::cout << std::endl << (*it).second << std::endl;
    
      switch((*it).first) {
        case  signal_namespace::SIG_PARENT_PROCESS_TERMINATE:
        case  signal_namespace::SIG_FORCEFUL_TERMINAL_CLOSE:
        case  signal_namespace::SIG_FORCEFUL_PROCESS_CLOSE:            
          break;
        
        default:
          bFatalError = true;
      }
    }
    
    pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eInformSignal, 0, sig);
    pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eStopApp);
  
    if(bFatalError)  {
      exit(0);
    }
  
  }


 /* This function is called to shutdown the application*/
  void clientShutdown()
  {
    std::cout << std::endl << "Client is closing, please wait..." << std::endl;
  
    if(pClient && pChatEvtCmd)
      pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eStopApp);
    
    std::cout << std::endl << "Client closed. Thanks." << std::endl;
  }



  int clientMain()
  {
  
    try
    {  
      std::cout << std::endl << "Trying to Connect " << pCmdLineParser->getIPAddress_string()  << ", Please wait.." << std::endl;        
      if( pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eStartApp)  != commondef_namespace::eSUCCESS )  {
        std::cout << std::endl << "Unable to connect to remote host, Connection refused" << std::endl;         
        return 0;
      }
  
      std::cout << std::endl << "Client Connected.. " << std::endl;
    
      pClient->attachCmdObject(pChatEvtCmd);    
      pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eHelpOptions);
    
      while(1)  {
        char *cPtr;
        int   iLen;
                
        char *buf = new char[SELECT_CLIENT_BUF_LEN];
        memset(buf,0, SELECT_CLIENT_BUF_LEN);
     
        (void)waitOnUserInput(buf, SELECT_CLIENT_BUF_LEN - 2, &iLen);
     
        if( (iLen > 0) && !(cPtr = (char *) memchr(&buf[0], 0x0A, iLen) ) ) {
          *(buf + iLen) = 0x0A;
        }
            
        if( pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::ePacketSend, buf, ++iLen)  ==  -1) {        
          delete buf;
        }
      
      } /*  while loop ends */ 
        
    }   /* try block ends */  
  
    catch(std::exception& e)  {
      std::cout << std::endl << e.what()  << std::endl;    
    } /* End of catch block */
 
    return 0;
  }



  int initMain()
  {  
    try 
    {    
      pClient = std::shared_ptr<chatClient>(new chatClient(pCmdLineParser)); /* The argument passed is number of thread needs to be created */
      pClient->registerLinuxSignalHandler(signalOccured); 
    
      pChatEvtCmd = std::shared_ptr<cChatEvtCmd>(new cChatEvtCmd(pClient));  /* Register the command interface */    
    
      return  clientMain();
    }
    catch(std::exception& e)  {
      std::cout << std::endl << e.what()  << std::endl;
    }
    catch(...)  {
      std::cout << std::endl << "Unexpected exception caught.." << std::endl;
    }
  
    clientShutdown(); /* This place could reach only due to exception, hence shutdown the client */
    return 0;
  }



  static void client_exit()
  { 
  
  }


} /* unamed namespace ends here */


  /*This is the main entry point for client application */
  int main(int argc, char** argv)
  { 
    atexit(client_exit);

    pCmdLineParser =   std::shared_ptr<cCmdLineParser>(new cCmdLineParser());
  
    if( pCmdLineParser  ==  nullptr) {
      std::cout << std::endl << "Not enough memory to execute the program, hence exiting.." << std::endl;
      exit(0);
    }
  
    if(argc < 3)  {
      pCmdLineParser->usage();
      return 0;
    }
  
    pCmdLineParser->parse_verify_encode_args(argc, argv);
    prepareSignalMsg();
  
    
    std::cout << std::endl << " IP Address : " << pCmdLineParser->getIPAddress_string();
    std::cout << std::endl << " Port No : " << pCmdLineParser->getPortNo_string();
    std::cout << std::endl << " Socket Type : " << pCmdLineParser->getSocketType_string();
    std::cout << std::endl << " Network domain : " << pCmdLineParser->getNetworkDomain_string() << std::endl;
      
    return initMain();    
  }

