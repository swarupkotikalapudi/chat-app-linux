#ifndef _COMMON_DEF_H
#include "common_def.h"
#endif
#ifndef _CCHATEVTCOMMAND_H
  #include "cChatEvtCmd.h"
#endif
#include<sys/wait.h>
#ifndef _CHAT_SERVER_H
  #include "chat_server.h"
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

#ifndef _C_CLOGGER_H
#include "cLogger.h"
#endif


#include<map>

#include <errno.h> //errno after a system call


std::shared_ptr<cCmdLineParser> pCmdLineParser;
std::shared_ptr<cChatApp>     pServer;
std::shared_ptr<IChatEvtCmd>     pChatEvtCmd;

cLogger* pLogger;

namespace
{
   using namespace socketHelper;
   
    const  int MAX_PORT_RETRY  = 5;
    const  int SELECT_SERVER_BUF_LEN = 1;
    std::map<int,std::string> strSignalMsg;
   
   /*This function is called when server is shutting down */
    void serverShutdown()
    {
      std::cout << std::endl << "Server is closing, please wait..." << std::endl;
      if(pServer && pChatEvtCmd) 
        pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eResourceClose);
  
      std::cout << std::endl << "Server closed. Thanks." << std::endl;
    }
    
    
    /*This function is called as the last function for the program terminating or closing */
    void server_exit()
    {
      cLogger::destroycLoggerSingleton();
    }
    
 
    /* this function is called when child process is closed, to avoid zombie process */ 
    void grimReaper(int sig)
    {
      int savedErrno;
      /* SIGCHLD handler to reap dead child processes */
      /* Save 'errno' in case changed here */
      savedErrno = errno;
      while ( waitpid(-1, NULL, WNOHANG)  > 0)
        continue;
    
 
      errno = savedErrno;
      
    }
    
    
    
    /* Registering a event when child process is terminate, to avoid zombie process */   
    void registerChildProcessKilled()
    {
      struct sigaction sa;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_RESTART;
      sa.sa_handler = grimReaper;
      if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("\n Error in sigaction call : ");
        exit(EXIT_FAILURE);
      }
    }
    
    
    
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
            /* do nothing, don't exit from here, app will trigger exit */
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
 
 
 
    int serverMain()
    {
      int   iRepeat = 0, iResult, iLen;
      char  cAns;
      bool  bRestart = true;
  
      while(1)  {
      try
      {
        if(bRestart)
        {
          std::cout << std::endl << "Server is starting, Please wait.." << std::endl;        
          if( ( iResult = pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eStartApp) ) != commondef_namespace::eSUCCESS )  {
            std::cout << std::endl << "Server starting failed..";
            throwException(iResult);
          }
        
          pServer->attachCmdObject(pChatEvtCmd);
                                         
          bRestart = false;
          std::cout << std::endl << "Server started.. " << std::endl;
          pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eHelpOptions);
        }
     
        waitOnUserInput(&cAns, SELECT_SERVER_BUF_LEN, &iLen);
      
        cAns = tolower(cAns);
                  
        if(cAns == 'p')  {
          pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::ePrintStats);
          continue;
        }
        else  if(cAns == 'q' )  {
          serverShutdown();
          break;
        }
        // else  if(cAns == 'r' )  {
        // cout << endl << "need to restart the server.." << endl;
        //pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eRestartApp);
        //}
    
      }   /* try block ends */
      catch(portNetWorkException& e) {
    
        std::cout << std::endl << "Port no or Host name wrong input is entered."  << std::endl;
        pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eResourceClose);
        
        if(iRepeat == MAX_PORT_RETRY) {
          std::cout << std::endl << "You have already attempted " << MAX_PORT_RETRY << " times changing, Server is shutting down." <<   std::endl;
          break;
        }
        else  {
          std::string sTmp( pCmdLineParser->getPortNo_string() ) ;      
          if( portNoExceptionHandler(sTmp) )  {
            networkExceptionHandler(sTmp);
            pCmdLineParser->setIPAddress_string( sTmp );        
          }
          else  {
            pCmdLineParser->setPortNo_string( sTmp );
          }
      
          iRepeat++;
          bRestart = true;
          continue;
        }
      }
      catch(std::exception& e)  {        
        std::cout << std::endl << e.what()  << std::endl;
        break; /* it is needed to break out of while loop */
      }/* End of catch block */
    
    } /* end of while loop */ 
 
  return 0;
}



  int initMain()
  {  
    try
    {  
      pServer = std::shared_ptr<chatServer>(new chatServer(pCmdLineParser));
        
      pChatEvtCmd = std::shared_ptr<cChatEvtCmd>(new cChatEvtCmd(pServer));  /* Register the command interface */
    
      pServer->registerLinuxSignalHandler(signalOccured);
      
      pLogger = cLogger::getLogger();
      
      return  serverMain();
    }
    catch(std::exception& e)  {
      std::cout << std::endl << e.what()  << std::endl;
    }
    catch(...)  {
      std::cout << std::endl << "Unexpected exception caught.." << std::endl;
    }
  
    serverShutdown(); /* This place could reach only due to exception, hence shutdown the server */
    return 0;
  }

}/* end of unnamed namespace */


  /*This is the main entry point for server application */
  int main(int argc, char** argv)
  { 
    atexit(server_exit);

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
    registerChildProcessKilled();
    
    std::cout << std::endl << " IP Address : " << pCmdLineParser->getIPAddress_string();
    std::cout << std::endl << " Port No : " << pCmdLineParser->getPortNo_string();
    std::cout << std::endl << " Socket Type : " << pCmdLineParser->getSocketType_string();
    std::cout << std::endl << " Network domain : " << pCmdLineParser->getNetworkDomain_string() << std::endl;
    
    return initMain();    
  }

