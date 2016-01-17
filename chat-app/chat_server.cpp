#ifndef _CHAT_SERVER_H
  #include "chat_server.h"
#endif
#ifndef _SOCKET_FACTORY_H
  #include "socket_factory.h"
#endif
#ifndef _CMSGQUEUE_H
  #include "cMsgQueue.h"
#endif
#ifndef _CSEMAPHORE_H
  #include "cSemaphore.h"
#endif

#ifndef _MQ_MSG_H
#include "mq_msg.h"
#endif

#include <netdb.h> //path name for /etc/services/ e.g. getaddrinfo() errror code etc..
#include<string.h>

#include<cxxabi.h>

#ifndef _C_CMDLINEPARSER_H
#include "cCmdLineParser.h"
#endif
#ifndef _C_CLOGGER_H
#include "cLogger.h"
#endif

#include<iostream>
#include<sstream>
#include <unistd.h>

/*chatServer constructor */
chatServer::chatServer(std::shared_ptr<cCmdLineParser> pCmdLineParser):cChatApp(pCmdLineParser) 
{
  errno = 0; /* clear error no */
  
  while(1)
  {
    if(P_setThreadSupportNeeded(2)  ==  -1) {     
      break;
    }
        
    pMsgQ_thrd1_S =   std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd1_R =   std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );    
    if( pMsgQ_thrd1_S->mq_Open("/m_Sock_server1") ==  -1) {
      break;
    }    
    if( pMsgQ_thrd1_R->mq_Open("/m_Sock_server1") ==  -1) {      
      break;
    }
        
    pMsgQ_thrd2_S =   std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd2_R =   std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );
    if( pMsgQ_thrd2_S->mq_Open("/m_Sock_server2")  ==  -1)  {      
      break;
    }  
    if( pMsgQ_thrd2_R->mq_Open("/m_Sock_server2") ==  -1) {      
      break;
    }
       
    /* Use semaphore for condition wait/post scenario */  
    m_pSemaphore =   std::unique_ptr<cSemaphore>(new cSemaphore(semaphore_namespace::eCONDITION));
      
    /* This thread does the a)server socket create b) wait for client connection c) messgage exchange with different clients */
    if( threadStart(pthread_namespace::eDETACH)  ==  -1) {/* thread 1 --  */      
      break;
    }
    
    /* This thread does the a)print client stats b) quit  */
    if(threadStart(pthread_namespace::eDETACH) ==  -1) { /* threas 2 -- */      
      break;
    }
    
    break; /* It reached here means no error happend, hence exit while loop */
  }
  
  if(errno)
    throw(strerror(errno));  
   
}


/*chatServer destructor */
chatServer::~chatServer()
{
}


/*chatServer Application initializer */
int chatServer::chatAppInit(int ListenQueueLength)
{
  socketFactory sFactory;
  
  m_eChatErrCode  = errno = 0;
  
  
  if( validateSocketArguments(m_pCmdLineParser->getNetworkDomain_number() , m_pCmdLineParser->getSocketType_number() ) != chatapp_namespace::eSUPPORTED ) {  
    return  commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
  }
    
  if( sFactory.factory_init(m_pCmdLineParser->getSocketType_number(), m_pCmdLineParser->getNetworkDomain_number(), commondef_namespace::eSERVER) == -1 )  {   
    return commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
  }
  
  /* factory method will create a socket, and will have a reference to tcp/udp communication type to do actual operation */
  pSocket = sFactory.createSocket( m_pCmdLineParser->getIPAddress_string(), m_pCmdLineParser->getPortNo_string() , ListenQueueLength);
  if(pSocket == nullptr ) {        
    m_eChatErrCode = errno;
    return m_eChatErrCode;
  }
  
  /* register with signal manager if any LINUX signal is generated, will be directed to ISocket class */
  registerSignalListner( pSocket );
  
  return commondef_namespace::eSUCCESS;
}


/*chatServer start */
int chatServer::startChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eSTART_APP;
  tMsg.iMsg  = 0;
  pMsgQ_thrd1_S->mq_Send(&tMsg);
  
  m_pSemaphore->sem_Wait(); /* Wait till application initialization finishes */
  /*Todo -- could be change it to be asynchronous, hence it return immediately and later when actual init of app finish some event is posted */
  
  
  return m_eChatErrCode; /* m_eChatErrCode is used as internal error storing mechansism when API are executed across different thread */
}


/*chatServer stop the server application */
int chatServer::stopChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eQUIT;
  tMsg.iMsg  = 0;
    
  pMsgQ_thrd2_S->mq_Send(&tMsg);  
  
  m_pSemaphore->sem_Wait();
   
  exit(0);
   
  return 0;
}


/*chatServer close resource e.g. socket etc.. */
int chatServer::resourceClose_ChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eRESOURCE_CLOSE;
  tMsg.iMsg  = 0;
    
  pMsgQ_thrd2_S->mq_Send(&tMsg);  
  
  m_pSemaphore->sem_Wait();
   
  return 0;
}


/*chatServer print stats about connected client's information */
int  chatServer::printConnectionStats()
{
  if(!pSocket)
    return -1;
  
  struct mqmsg_namespace::sockMsg tMsg;
  tMsg.msgID = mqmsg_namespace::ePRINT_STATS;
  tMsg.iMsg  = 0;
  pMsgQ_thrd2_S->mq_Send(&tMsg);
  return 0;
  
}

/*This thread has two function a) server socket init b) wait for client connection */
void chatServer::thread_handler1()
{
  bool bRun = true;
  struct mqmsg_namespace::sockMsg tMsg;
  
  while(bRun)
  {
    try
    {
      memset(&tMsg, 0, sizeof(struct mqmsg_namespace::sockMsg) );
      pMsgQ_thrd1_R->mq_Recv(&tMsg);
      switch(tMsg.msgID)
      {
        case mqmsg_namespace::eSTART_APP:
          /*Socket_Connect is polymorphic function, based on socket communication type TCP/UDP connect will be invoked.. */
          
          m_eChatErrCode = 0;
          if( pSocket->Socket_Init() != 0 )  {          
            m_eChatErrCode = ( errno ? errno : EINVAL); /* errno could change by OS if any error occured in the system call */            
            
            m_pSemaphore->sem_Post();                   /* socket_init has failed hence wait again on message queue */  
            continue;
          }
          
          m_pSemaphore->sem_Post();
          
          /* socket descriptor is acquired, Socket_Process_Clients() function will wait for new client connection requests */       
          if( pSocket->Socket_Process_Clients() < 0 ) {
            /* Socket_Process_Clients returns due to client connection close.. */
          }
          
          if(errno == ENOTCONN) {
            
            onClientConnectionClosed(getpid()); /* client is closed/disconnected, hence child process at server need to close.. */
          }
          else  {
            continue; /* some other error occured, wait for new message */    
          }
          
          bRun = false;
          break;
      
        //case eSTOP_THREAD:
          //bRun = false;
          //break;
        
        default:
          break;
      } /* switch ends here */
    } /* try ends here */
    catch(abi::__forced_unwind&)
    {
      throw;
    }
    catch(std::exception& e)  {      
      std::cout << std::endl <<  e.what() << std::endl;
      onFatalErrorOccured(errno);
    }
    catch(...)  {
      onFatalErrorOccured(errno);
      
    } /* catch ends here */
  }  /*while loop ends here */

}


/*chatServer -- this thread used a)print stats b)quit server app c)clean up of client when client is closed etc.. */
void chatServer::thread_handler2()
{
  bool bRun = true;
  struct mqmsg_namespace::sockMsg tMsg;
  
  while(bRun)
  {
    try
    {
      memset(&tMsg, 0, sizeof(struct mqmsg_namespace::sockMsg) );
      pMsgQ_thrd2_R->mq_Recv(&tMsg);
    
      switch(tMsg.msgID)
      {
        case  mqmsg_namespace::eCHILD_PROC_KILLED:          
          pSocket->Socket_Erase_Child_Process_Details(tMsg.iMsg);
          
          break;
        
        case mqmsg_namespace::ePRINT_STATS:
          pSocket->Socket_printConnectionStats();          
          break;
          
          
        case mqmsg_namespace::eRESOURCE_CLOSE:
          if(pSocket) {           
            pSocket->Socket_Shutdown();
            pSocket->appShutdown();
         
            unregisterSignalListner( pSocket );
            pSocket.reset();
          }
          m_pSemaphore->sem_Post();
          break;
          
        case mqmsg_namespace::eQUIT:
                    
          if(pSocket) {           
            pSocket->Socket_Shutdown();
            pSocket->appShutdown();
         
            unregisterSignalListner( pSocket );
            pSocket.reset();
          }
          
          //bRun = false;
          m_pSemaphore->sem_Post();
          break;
      
        default:
          std::cout << std::endl << "default message received.." << std::endl;
          break;
      }
    } /* try blocks ends here */
    catch(abi::__forced_unwind&)
    {
      throw;
    }
    catch(std::exception& e)  {      
      std::cout << std::endl <<  e.what() << std::endl;
      onFatalErrorOccured(errno);
      continue;
    }
    catch(...)
    {
      onFatalErrorOccured(errno);
      continue;
    }
        
  } /* while loop continues */
}


/*chatServer -- callback function called by os runtime environment when thread is created */ 
void chatServer::thread_handler(int iThreadIndex)
{
  /*
    Note: 1. create two thread..
      1st thread receive client request to start server 
      
      2nd thread wait for command e.g. a)print stats b)child killed c)quit d)restart server etc...
  */
  
  switch(iThreadIndex)  {
    case 1:
      thread_handler1();
      break;
    
    case 2:
      thread_handler2();
      break;
      
    default:
      break;
  }  
}


/*chatServer -- this function is called when client connection with server is lost */
void chatServer::onClientConnectionClosed(int iPid)
{ 
  struct mqmsg_namespace::sockMsg tMsg;
   
  std::cout << std::endl << "client " <<  iPid << " is closed..." << std::endl;
    
  tMsg.msgID = mqmsg_namespace::eCHILD_PROC_KILLED;
  tMsg.iMsg  = getpid();
  pMsgQ_thrd2_S->mq_Send(&tMsg);
  
}


/* chatServer -- can be used to print help options.. */
void  chatServer::printHelpOptions()
{ 
  std::cout << std::endl << "Please press any of below options : " << std::endl;
  std::cout << "\tPress p to print connected client statistics.." << std::endl;
  //std::cout << "\tPress r to restart the server.." << std::endl;
  //std::cout << "\tPress h to help options.." << std::endl;
  std::cout << "\tPress q to quit.." << std::endl;
}


/*chatServer -- shared object is used so that socket connection can callback the callback function on server side.. */
int  chatServer::attachCmdObject(std::shared_ptr<IChatEvtCmd> pEvtCmd)
{ 
  if( pEvtCmd )
    pSocket->Socket_attachCommandObject(pEvtCmd);
  else
    throw "Command object initialization failed..";
  
  return 0;
}


/*chatServer -- todo -- this need to be fixed as server doesn't need this function.. */
void  chatServer::sendPacket(const  char *buf, const int bufLen)
{

}

/*chatServer -- any information recvd from client connection */
void  chatServer::recvdPacket(std::stringstream& sStream)
{
  std::cout <<  "client id " << getpid() << " :\t";
    std::cout << sStream.str();
    std::cout << std::endl;
}


/*chatServer -- if any exception happens in any of the threads, this function is called and server is closed immediately. */
void chatServer::onFatalErrorOccured(int errNo)
{
  std::cout << std::endl << "Internal error occured : " << strerror(errNo) << std::endl;  
  exit(0);
}

