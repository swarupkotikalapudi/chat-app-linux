#ifndef _CHAT_CLIENT_H
  #include "chat_client.h"
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

#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif

#include <netdb.h> //path name for /etc/services/ e.g. getaddrinfo() errror code etc..
#include<string.h>

#include<cxxabi.h>

#ifndef _C_CMDLINEPARSER_H
#include "cCmdLineParser.h"
#endif

#include<sstream>

chatClient::chatClient(std::shared_ptr<cCmdLineParser> pCmdLineParser):cChatApp(pCmdLineParser) 
{
  errno = 0;
  
  while(1)
  {
    if(P_setThreadSupportNeeded(4)  ==  -1)
      break;
        
    pMsgQ_thrd1_S =   std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd1_R =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );    
    if( pMsgQ_thrd1_S->mq_Open("/m_Sock_client1") ==  -1)
      break;    
    if( pMsgQ_thrd1_R->mq_Open("/m_Sock_client1") ==  -1)
      break;
    
    pMsgQ_thrd2_S =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd2_R =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );    
    if( pMsgQ_thrd2_S->mq_Open("/m_Sock_client2") ==  -1)
      break;
    if( pMsgQ_thrd2_R->mq_Open("/m_Sock_client2") ==  -1)
      break;
      
    pMsgQ_thrd3_S =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd3_R =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );
    
    if( pMsgQ_thrd3_S->mq_Open("/m_Sock_client3") ==  -1)
      break;      
    if( pMsgQ_thrd3_R->mq_Open("/m_Sock_client3") ==  -1)
      break;
            
    pMsgQ_thrd4_S =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::SEND_MODE | mqmsg_namespace::CREATOR_MODE) ); 
    pMsgQ_thrd4_R =    std::unique_ptr<cMsgQueue>(new cMsgQueue(mqmsg_namespace::RECV_MODE) );
    if( pMsgQ_thrd4_S->mq_Open("/m_Sock_client4") ==  -1)
      break;
    if( pMsgQ_thrd4_R->mq_Open("/m_Sock_client4") ==  -1)
      break;
    
    /* Use semaphore for condition wait/post scenario */  
    m_pSemaphore =    std::unique_ptr<cSemaphore>(new cSemaphore(semaphore_namespace::eCONDITION)); 
    
    /* Use semaphore for condition wait/post scenario for thread for packet recv */
    m_pPacktRecvd_Semaphore =  std::unique_ptr<cSemaphore>( new cSemaphore(semaphore_namespace::eCONDITION) );
    
    /* Use semaphore for condition wait/post scenario for thread for packet send  */
    m_pPacktSend_Semaphore =  std::unique_ptr<cSemaphore>( new cSemaphore(semaphore_namespace::eCONDITION) );
           
    /* This thread does the a)server socket create b) wait for client connection c) messgage exchange with different clients */
    if( threadStart(pthread_namespace::eDETACH)  ==  -1) /* thread 1 --  */
      break;
      
    /* This thread does the a)print client stats b) quit  */
    if( threadStart(pthread_namespace::eDETACH)  ==  -1)  /* threas 2 -- */
      break;

    /* this thread receive message from server to client  */    
    if(threadStart(pthread_namespace::eDETACH) ==  -1) /* thread 3 --  */
      break;
      
    /* this thread send message from  client to server */
    if( threadStart(pthread_namespace::eDETACH)  ==  -1) /* thread 4 --  */
      break;
    
    break; /* exit from loop, as construction finished */      
  }
  
  if(errno)
    throw inputValidationException( strerror(errno) );   
    
}



chatClient::~chatClient()
{  
}


/*chatClient Application initializer */
int chatClient::chatAppInit(int ListenQueueLength)
{
  socketFactory sFactory;
  m_eChatErrCode  = errno = 0;
    
  if( validateSocketArguments(m_pCmdLineParser->getNetworkDomain_number() , m_pCmdLineParser->getSocketType_number() ) != chatapp_namespace::eSUPPORTED ) {
    return  commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
  }
    
  if( sFactory.factory_init(m_pCmdLineParser->getSocketType_number(), m_pCmdLineParser->getNetworkDomain_number(), commondef_namespace::eCLIENT) == -1 )  {
    return commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED;
  }
  
  /* factory method will create a socket, and will have a reference to tcp/udp communication type to do actual operation */
  pSocket = sFactory.createSocket( m_pCmdLineParser->getIPAddress_string(), m_pCmdLineParser->getPortNo_string(), 0 );
  if(pSocket == nullptr ) {        
    m_eChatErrCode = errno;
    return m_eChatErrCode;
  }
  
  /* register with signal manager if any LINUX signal is generated, will be directed to ISocket class */
  registerSignalListner( pSocket );
  
  return commondef_namespace::eSUCCESS;
}


/*chatClient start */
int chatClient::startChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eSTART_APP;
  tMsg.iMsg  = 0;
  pMsgQ_thrd1_S->mq_Send(&tMsg);
  
  m_pSemaphore->sem_Wait();
  
  if(m_eChatErrCode == 0) {
    /*Socket is initialize, hence signaling read and write thread to ready to receive messages  */
    m_pPacktRecvd_Semaphore->sem_Post();
    m_pPacktSend_Semaphore->sem_Post();
  
    tMsg.msgID = mqmsg_namespace::ePACKET_RECV_START;
    tMsg.iMsg  = 0;
    pMsgQ_thrd3_S->mq_Send(&tMsg);
  }
  
  return m_eChatErrCode; /* m_eChatErrCode is used as internal error storing mechansism when API are executed across different thread */
}


/*chatClient stop */
int chatClient::stopChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eQUIT;
  tMsg.iMsg  = 0;
    
  pMsgQ_thrd2_S->mq_Send(&tMsg);
  
  m_pSemaphore->sem_Wait();
  
  exit(0);
  
  return 0;
}


int chatClient::resourceClose_ChatApp()
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::eRESOURCE_CLOSE;
  tMsg.iMsg  = 0;
    
  pMsgQ_thrd2_S->mq_Send(&tMsg);  
  
  m_pSemaphore->sem_Wait();
   
  return 0;
}


/*chatClient -- this thread is used to show client stats  */
int  chatClient::printConnectionStats()
{
  if(pSocket) {
    struct mqmsg_namespace::sockMsg tMsg;
    tMsg.msgID = mqmsg_namespace::ePRINT_STATS;
    tMsg.iMsg  = 0;
     
    pMsgQ_thrd2_S->mq_Send(&tMsg);
  }
  return 0;
}

/*chatClient -- this thread is used during socket initialization */
void chatClient::thread_handler1()
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
          if( pSocket->Socket_Init() != 0)       
            m_eChatErrCode  = (errno ? errno : ECONNREFUSED); /* errno could change by OS if any error occured in the system call */
                   
          m_pSemaphore->sem_Post();
                                                 
          /* socket_init has failed hence wait again on message queue */  
          continue;
          break;
      
        case mqmsg_namespace::eSTOP_THREAD:
          bRun = false;
          break;
        
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


/*chatClient -- this thread is used to quit the client application */
void chatClient::thread_handler2()
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
        
        case mqmsg_namespace::ePRINT_STATS:
          pSocket->Socket_printConnectionStats();
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
    }
    catch(...)
    {      
      onFatalErrorOccured(errno);
    }
        
  } /* while loop continues */
}


/*chatClient -- this thread is used to recv packets from  remote server */
void chatClient::thread_pkt_recv_handler()
{
  bool bRun = true;
  struct mqmsg_namespace::sockMsg tMsg;
  
  m_pPacktRecvd_Semaphore->sem_Wait();
  
  while(bRun)
  {
    try
    {
      memset(&tMsg, 0, sizeof(struct mqmsg_namespace::sockMsg) );
      pMsgQ_thrd3_R->mq_Recv(&tMsg);
      switch(tMsg.msgID)
      {
        case mqmsg_namespace::ePACKET_RECV_START:
          /* as this a client connection, the packet from server will be received or executed in this thread context */
          pSocket->Socket_doPacketRecv();
          std::cout << std::endl << "Connection closed by server." << std::endl;
          stopChatApp();
          //bRun = false;            
          break;
          
        default:
        break;
      }
    }
    catch(abi::__forced_unwind&)
    {
      throw;
    }
    catch(...)
    {
      onFatalErrorOccured(errno);
    }
  
  }/* while loop ends  */
  
}


/*chatClient -- this thread is used to send information from command line to remote server */
void chatClient::thread_pkt_send_handler()
{
  bool    bRun = true;
  struct  mqmsg_namespace::sockMsg tMsg;
  
  m_pPacktSend_Semaphore->sem_Wait();
  
  while(bRun)
  {
    try
    {      
      memset(&tMsg, 0, sizeof(struct mqmsg_namespace::sockMsg) );
      pMsgQ_thrd4_R->mq_Recv(&tMsg);
     
      switch(tMsg.msgID)      
      {
        case mqmsg_namespace::ePACKET_SEND_START:
          {
            /*Client side, the message for the server are send from here.. */ 
                      
            pSocket->Socket_doPacketSend((const char *) (tMsg.pBuf), tMsg.iMsg);
                      
            //todo if server write fails app should close..
            //cout << endl << "Connection closed by server." << endl;
            //stopChatApp();
          }
          
          break;
          
        default:
        break;
      }
    }
    catch(abi::__forced_unwind&)
    {
      throw;
    }
    catch(...)
    {
      onFatalErrorOccured(errno);
    }
  
  }/* while loop ends  */
  
}


/*chatClient -- callback function called by os runtime environment when thread is created */ 
void chatClient::thread_handler(int iThreadIndex)
{
  /*
    Note: 1. create four thread..
      1st thread receive client socket creation request
      2nd thread wait for command e.g. a)print stats 2)quit command 
      3rd thread to listen for incoming packet from server
      4th thread to send outgoing packet to server
  */
  
  switch(iThreadIndex)  {
    case 1:
      thread_handler1();
      break;
    
    case 2:
      thread_handler2();      
      break;
    
    case 3:      
      thread_pkt_recv_handler();     
      break;
    
    case 4:
      thread_pkt_send_handler();      
      break;
      
    default:
     /* this case should not execute */
      break;
  }
}


/*chatClient -- print help options */
void  chatClient::printHelpOptions()
{ 
  std::cout << std::endl << "Please press any of below options : " << std::endl;
  std::cout << "\tPress ctrl c to quit" << std::endl;
}


/*chatClient -- shared object is used so that socket connection can callback the callback function on client side.. */
int  chatClient::attachCmdObject(std::shared_ptr<IChatEvtCmd> pEvtCmd)
{
  pSocket->Socket_attachCommandObject(pEvtCmd);      
  return 0;
}


/*chatclient --  information from command line send to server */
void  chatClient::sendPacket(const  char *bufMsg, const int bufLen)
{
  struct mqmsg_namespace::sockMsg tMsg;
    
  tMsg.msgID = mqmsg_namespace::ePACKET_SEND_START;
  tMsg.iMsg  = bufLen;
  tMsg.pBuf  = (const char *) bufMsg;
    
  pMsgQ_thrd4_S->mq_Send(&tMsg);
    
}


/*chatClient -- information recvd from server connection */
void  chatClient::recvdPacket(std::stringstream& sStream)
{  
  std::cout << sStream.str();
  std::cout << std::endl;
}


/*chatClient -- if any exception happens in any of the threads, this function is called and server is closed immediately. */
void chatClient::onFatalErrorOccured(int errNo)
{
  std::cout << std::endl << "Internal error occured : " << strerror(errNo);
  exit(0);
}

