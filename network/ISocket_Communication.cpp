#ifndef _ISOCKET_COMMUNICATION_H
  #include "ISocket_Communication.h"
#endif
#ifndef _ISOCKET_ADDRESS_H
  #include "ISocket_Address.h"
#endif
#ifndef _CSEMAPHORE_H
  #include "cSemaphore.h"
#endif
#ifndef _CSHAREDMEM_H
#include "cSharedMem.h"
#endif

#ifndef _ICHATEVTCOMMAND_H
#include "IChatEvtCmd.h"
#endif

//#include "IPacket.h"
#include "cTextPacket.h"

#ifndef _CMUTEXCONDWAIT_H
#include "cMutexCondwait.h"
#endif

#ifndef _C_CLOGGER_H
#include "cLogger.h"
#endif


#include<iostream>

#include <netdb.h> //path name for /etc/services/ e.g. getaddrinfo() errror code etc..
#include<sys/socket.h>
#include<sys/wait.h>
#include<string.h>

#include<cxxabi.h>

#include<sstream>

#include <errno.h> //errno after a system call

#include <unistd.h>

namespace
{
  const char* SERVER_MESSAGE      = "Hello -- ";
  const int   SERVER_MESSAGE_LEN  =  45;
  const int   ADDRSTRLEN          =  (NI_MAXHOST + NI_MAXSERV + 10);
}

ISocketCommunication::ISocketCommunication(void): P_Thread(),sock_fd(0), m_iCntClientConnections(0), m_pSocketAddress(nullptr)
{  
  /* binary semaphore used, which will be similar to mutex, it can be locked/unlocked from different thread and related process as well */
  m_pSemaphore =   std::unique_ptr<cSemaphore>(new cSemaphore(semaphore_namespace::eLOCK) );
  
  m_pMutexReadCondwait  =   std::unique_ptr<cMutexCondwait>(new cMutexCondwait() );
  m_pMutexWriteCondwait  =   std::unique_ptr<cMutexCondwait>(new cMutexCondwait() ); 
    
  
  
  m_recvOfs = m_recvBegin = 0;
  m_sendOfs = 0;
  
  m_bReadyToRead_InBndPkts  = true;
  m_bReadyToSend_OutBndPkts = true;
  
  m_bThreadCreated  = false;
  
  memset(m_recvBuf, 0, sizeof(m_recvBuf) );
  
}



ISocketCommunication::~ISocketCommunication(void)
{
}



void ISocketCommunication::setSocketAddress(std::unique_ptr<ISocketAddress> pSocketAddress)
{
  m_pSocketAddress = std::move(pSocketAddress);
}



void ISocketCommunication::getTimestamp(char* strTimeStamp)
{
  time_t l_time;
  struct tm result;
  if( !strTimeStamp ) return;
  
  l_time = time(NULL);
  localtime_r(&l_time, &result); 
  asctime_r(&result, strTimeStamp);
}



int ISocketCommunication::Socket_ServerHeartbeatMsg()
{
  int iRet, msgSize;
  char  stime[32] = {0};
  const char *pcNullDelimeter;
  char *srvMsg = new char[SERVER_MESSAGE_LEN];
  
  getTimestamp(stime);
  memset(srvMsg, 0, SERVER_MESSAGE_LEN);
  snprintf(srvMsg, SERVER_MESSAGE_LEN, "%s %s", SERVER_MESSAGE, stime);
  
  pcNullDelimeter = static_cast<char *>( memchr(&srvMsg[0], 0, SERVER_MESSAGE_LEN) );
  
  msgSize = pcNullDelimeter - &srvMsg[0];
  *(srvMsg + msgSize - 1) =  0x0A; /* this application is using newline as delimeter for message, hence adding the delemeter */
  *(srvMsg + msgSize ) =  0x0;
  
  iRet = Socket_doPacketSend(srvMsg, msgSize);
  
  return iRet;
}



int ISocketCommunication::Socket_Erase_Child_Process_Details(pid_t iPid)
{
  std::map<pid_t, int>::iterator itProcess;
  m_pSemaphore->sem_Wait();
  
  itProcess = m_childProcessMap.find( iPid );
  if(itProcess != m_childProcessMap.end())  {    
    m_childProcessMap.erase(itProcess);
    m_iCntClientConnections--;
  }
  
  /* Shared memory object is shared between different processes, however m_childProcessMap and m_iCntClientConnections is maintain only by main process, hence below
    statement is not part of above if condition check... */
  m_psharedMem->sharedMem_removeClient(iPid);
   
  m_pSemaphore->sem_Post();
  
  return 0;
}



int ISocketCommunication::appShutdown()
{
  std::map<int,int>::iterator it;
    
  m_pSemaphore->sem_Wait();
        
  for(it = m_childProcessMap.begin(); it!= m_childProcessMap.end(); it++) {
    kill( (*it).first, SIGTERM );
        
    waitpid((*it).first, nullptr,WNOHANG);
  }
      
  m_pSemaphore->sem_Post();
       
  return 0;
}

/* This function print server's stats information */
void  ISocketCommunication::printServerConnectionInfo()
{
  std::cout << std::endl;
  m_pSemaphore->sem_Wait();
  m_psharedMem->sharedMem_printClientStats();
  m_pSemaphore->sem_Post();
  std::cout << std::endl;
}


/*This function represent each client connection handling from server's point of view */
void ISocketCommunication::handleClientRequest()
{
  try
  {
    
    if( threadStart(pthread_namespace::eDETACH) == -1) {      
      throw("Thread creation failed");
      return;
    }
  
    if( threadStart(pthread_namespace::eDETACH) == -1) {      
      throw("Thread creation failed");
      return;
    }
  
    m_bThreadCreated  = true;
    
    Socket_ReceiveInboundPackets();
  }
  catch(abi::__forced_unwind&)
  {
    throw;
  }
  catch(...)
  {
    
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eChildRemoved, nullptr, getpid() );      
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eFatalError, nullptr, errno);
    
  }
  
}


/*This is the function on which server wait for new client connection. */
int ISocketCommunication::Socket_Process_Clients(void)
{
  pid_t  ichild_process_id;
  struct sockaddr_storage client_address;
  socklen_t addrlen;
  char addrStr[ADDRSTRLEN] = {0}, host[NI_MAXHOST] = {0}, service[NI_MAXSERV] = {0};
  std::map<pid_t,int>::iterator it;
  int client_sock_fd = 0;
   
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }
  
  /*  This chat server is create one child process for each client request.
      All the child process are grouped under parent process. Hence parent and all child proecss are treated as one job in Linux/Unix OS
      If parent is terminated due to any signal (e.g. ctrl c), OS will kill, all the child processes.  */
  setpgid(getpid(), getpid());
  
  addrlen = sizeof(struct sockaddr_storage);

  for (;;) {
    memset(&client_address, 0, sizeof(struct sockaddr_storage) );
    /* Wait for connection from client */
    client_sock_fd = accept(sock_fd, (struct sockaddr *) &client_address, &addrlen) ;
    
    if(client_sock_fd == -1 || errno)
    {
      if(errno == EINTR)  {
        continue;
      }
      else { //if(errno == EBADF || errno == EINVAL)        
        errno = ENOTCONN;
        break;
      }
    }
    
    if (getnameinfo((struct sockaddr *) &client_address, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
      snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
    else
      snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
     
    if(m_iCntClientConnections ==  socketCommunication_namespace::MAX_CLIENT_CONNECTION) {
      std::cout << std::endl << "Client Connection from : " << addrStr << " is refused, as maximum supported client is reached. " << std::endl;
      close(client_sock_fd); /* As maximum supported clients is reached, no more new client connection are allowed.. */
      continue;
    }
      
    /* Handle each client request in a new child process */
    switch (ichild_process_id = fork() ) {
      case -1:
        perror("\n Fork creation : ");
        close(client_sock_fd); /* Give up on this client */
        break;/* May be temporary; try next client */
      
      case 0: /* Child */
        
        std::cout << std::endl << "Client Connection from : " << addrStr << std::endl;
        
        close(sock_fd); /* Unneeded copy of listening socket */
        sock_fd =  client_sock_fd;                       
        handleClientRequest();
        
        /*two threads(for (Rx/Tx)) created for each client connection, This has a design of one process for each client connection, internally for each client process, two new thread are created, if  client connection is lost for any reason, these two thread have to be cancelled, so that process can be closed and release its memory.. */
        if(m_bThreadCreated)  {
          thread_cancel(0);
          thread_cancel(1);
        }
        
        break;
         
      default: /* Parent */
        char buf[128] = {0};
        m_pSemaphore->sem_Wait();
               
        m_childProcessMap.insert( std::make_pair(ichild_process_id, client_sock_fd) );
        m_iCntClientConnections++;
                                        
        m_psharedMem->sharedMem_addClient(ichild_process_id);
        
        m_pSemaphore->sem_Post();
        
        sprintf(buf, "Child is created : %d", ichild_process_id);                
        m_pLogger->log_write(buf);
                      
        /* If client requires, client can be informed events like new client connection established etc... */
        //if(m_pChatEvtCmd)
          //m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eNewClientCreated);
                        
        close(client_sock_fd); /* Unneeded copy of connected socket */
        break; /* Loop to accept next connection */
    } /*End of switch case */
    
  }   /*End of for loop */
  
  
  return 0;
}



ssize_t ISocketCommunication::Socket_Write_helper(int sfd, const void *buffer, size_t n)
{
  ssize_t numWritten; /* # of bytes written by last write() */  
  size_t totWritten;  /* Total # of bytes written so far */
  const char *buf;
  
  if(sfd <= 0)  {
    return -1;
  }
  
  buf = (char *) buffer; /* we typecase the 'void *' on char pointer */
  
  for (totWritten = 0; totWritten < n; ) {
    numWritten = write(sfd, buf, n - totWritten);
    if (numWritten <= 0) {
      if (numWritten == -1 && errno == EINTR)
        continue; /* Interrupted --> restart write() */
      else
        return -1;  /* Some other error */
    }
    totWritten += numWritten;
    buf += numWritten;
    
    m_pSemaphore->sem_Wait();
    m_psharedMem->sharedMem_UpdateClient(getpid(), numWritten, 0);
    m_pSemaphore->sem_Post();    
  }
  
  return totWritten;  /* Must be 'n' bytes if we get here */  
}



int ISocketCommunication::Socket_Close_helper(int sfd)
{
  if(sfd) {
    close(sfd);
    sfd = 0;
  }

  return 0;
}



int  ISocketCommunication::Socket_Shutdown_helper(int sfd, int how)
{
  if(sfd) {
    shutdown(sfd, how);    
    sock_fd = 0;
  }
 
  return 0;
}



int ISocketCommunication::Socket_Close(void)
{
  if(sock_fd) {
    close(sock_fd);
    sock_fd = 0;
  }
  
  return 0;
}



int ISocketCommunication::Socket_Shutdown()
{
  if(sock_fd)
    Socket_Shutdown_helper(sock_fd);   
  return 0;
}



int ISocketCommunication::Socket_attachCommandObject(std::shared_ptr<IChatEvtCmd> pEvtCmd)
{
  if(m_pChatEvtCmd)
    m_pChatEvtCmd.reset();
  
  m_pChatEvtCmd = pEvtCmd;
    
  return 0;
}


/* This function used by client to receive packets on a thread */
int ISocketCommunication::Socket_doPacketRecv()
{
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }
  
  return Socket_ReceiveInboundPackets();
}


/* This function is mainly called by client to send message over the network.. */
int ISocketCommunication::Socket_doPacketSend(const char *msgBuf, const int bufLen )
{
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }  
  
  /* it search for a newline character as a delimeter and  creates a message */
  char* pcNewlineDelimter = (char *)( memchr(&msgBuf[0], 0x0A, bufLen) ); /* 0x0A ascii value of new line '\n' */
  
  if(pcNewlineDelimter) {
    
    char t = *(pcNewlineDelimter + 1);
    
    *(pcNewlineDelimter + 1) =  0x0; /* null terminator is must here, for message boundry */    
    std::shared_ptr<cTextPacket> pkt(new cTextPacket( &msgBuf[0] )  );
    
    m_pMutexWriteCondwait->lock();
    m_OutboundPacket.push_back(pkt);  
    m_pMutexWriteCondwait->unlock();
    m_pMutexWriteCondwait->mutex_cond_signal();
      
    *(pcNewlineDelimter + 1) = t;
  }
  else  {
    /* Ideally this case should not occur, just put for debug purpose.. */
    std::cout << std::endl << "outbound packet is not inserted.. " << "\t" << msgBuf << std::endl;
  }  
  
  delete[] msgBuf;
  
  m_pMutexWriteCondwait->mutex_cond_signal();
  
  return 0;
}


/*This function runs under a seprate thread, whose job is to send over network each message.. */
int ISocketCommunication::Socket_HandleOutboundPackets()
{
  const char *buf;
  int  iSend = 0, rc, iPktCnt, pktSize;
  std::shared_ptr<IPacket> pkt;
  
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }
  
  try
  {    
    while(1)
    {
      m_pMutexWriteCondwait->lock();
      while( ( m_OutboundPacket.empty() ) && m_bReadyToSend_OutBndPkts) {
            
        m_pMutexWriteCondwait->mutex_cond_wait();
      }
      m_bReadyToSend_OutBndPkts = false;
      iPktCnt = m_OutboundPacket.size();
      m_pMutexWriteCondwait->unlock();
          
      do
      {
        pkt = *m_OutboundPacket.begin();            
        buf = pkt->packet_data();
        
        pktSize = static_cast<int>(pkt->packet_size());            
        m_OutboundPacket.pop_front(); 
        iPktCnt--;
        
        errno = 0;
        if( ( rc = Socket_Write_helper(sock_fd, buf + m_sendOfs  , pktSize - m_sendOfs ) ) > 0 )  {
          m_sendOfs += rc;
          iSend      = 1;
        }
        else  if(rc == -1 && errno) {
          /*Error during write, hence exit  */
          
          return -1;
        }
        else  {
          iSend = 0;
        }

        if(m_sendOfs == pktSize ) {          
          m_sendOfs = 0;
        }
    
      } while(iSend && iPktCnt);
                
      m_bReadyToSend_OutBndPkts = true;
    }
  }
  catch(abi::__forced_unwind&)
  {
    throw;
  }
  catch(...)
  {
          
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eChildRemoved, nullptr, getpid() );      
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eFatalError, nullptr, errno);
        
  }
   
  return 0;
}


/*This function is called from thread context, where the only job is to process each packet from list */
int ISocketCommunication::Socket_ProcessInboundPackets()
{
  int iPktCnt;
  std::shared_ptr<IPacket> packet;
  
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }
  
  try
  {
    while(1)  {
                              
      m_pMutexReadCondwait->lock();
    
      while( ( m_InboundPacket.empty() ) && m_bReadyToRead_InBndPkts) {
     
        m_pMutexReadCondwait->mutex_cond_wait();
      }
      m_bReadyToRead_InBndPkts  = false;
      iPktCnt = m_InboundPacket.size();
    
      m_pMutexReadCondwait->unlock();
                  
      do
      {
        
        packet = *m_InboundPacket.begin();
                
        std::stringstream tStream(packet->packet_data());
        m_pChatEvtCmd->onStreamRecvd(tStream);
        
        m_InboundPacket.pop_front();
        iPktCnt--;
        
        /* for each received packet from client, one heart beat message (hello world + time stamp) is sent to client */
        if(m_eConnectionType == socketCommunication_namespace::eSockServer)  {
          Socket_ServerHeartbeatMsg();
        }
                                                  
      }while(iPktCnt);
        
      m_bReadyToRead_InBndPkts  = true;  
    }
  }
  catch(abi::__forced_unwind&)
    {
      throw;
    }
  catch(...)  {
      
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eChildRemoved, nullptr, getpid() );      
    m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eFatalError, nullptr, errno);
    
  }
 
  /*theortically, it should not reach here, except if any exception error happens */ 
  return  0;
}


/* this function wait for incoming packets, and add these packet to list, another thread read those packets */
int ISocketCommunication::Socket_ReceiveInboundPackets()
{
  bool    bPktReceived = false;
  u_long  packetSize = 0;
  int newData;
  ssize_t numRead;
  char *pcNewlineDelimeter;
  
  if(sock_fd <= 0)  {
    return -1; /* socket has not been initialized, hence return with error.. */
  }
  
  while( (numRead = recv(sock_fd, m_recvBuf + m_recvBegin + m_recvOfs,  socketCommunication_namespace::RECV_BUFFER_SIZE  - m_recvOfs, 0) ) )
  {
    if (numRead == -1) {
      if (errno == EINTR) { /* Interrupted hence restart read() */
        continue;
      }
      else  { /* error may have occured */
        /* todo what to do if any remaining or lefover message in the list ?? */
        std::cout << std::endl << "Remote host connection lost " << std::endl;
        return -1;
      }
    }
    
    m_pSemaphore->sem_Wait(); 
    m_psharedMem->sharedMem_UpdateClient(getpid(), 0, numRead); /* update the shared memory about no of byte received (Rx) */
    m_pSemaphore->sem_Post();
          
    newData = m_recvOfs + numRead;
    bPktReceived = false;
    
    while( newData > 0)
    {                  
      /* it search for a newline character as a delimeter and  creates a message */
      pcNewlineDelimeter = static_cast<char *>( memchr(&m_recvBuf[m_recvBegin], 0x0A, numRead) ); /* 0x0A ascii value of new line '\n' */
      
      if( pcNewlineDelimeter ) {
        packetSize = pcNewlineDelimeter - &m_recvBuf[m_recvBegin] + 1 ;                  
      }
      else  { /* delimeter not found so treat the whole message as one packet */                
        pcNewlineDelimeter  = &m_recvBuf[m_recvBegin + newData + 1 ];
        packetSize  = newData;
      }
        
      char t = *(m_recvBuf + packetSize + m_recvBegin);
      *(m_recvBuf + m_recvBegin + packetSize ) = 0x0;
      std::shared_ptr<IPacket> pkt( new cTextPacket(&m_recvBuf[m_recvBegin])  );
      *(m_recvBuf + m_recvBegin + packetSize ) = t;
                        
      m_pMutexReadCondwait->lock();
      m_InboundPacket.push_back(pkt);
      m_pMutexReadCondwait->unlock();
      m_pMutexReadCondwait->mutex_cond_signal();
              
      memset(&m_recvBuf[m_recvBegin + m_recvOfs], 0, (packetSize - 1) ); /* set to zero the buffer which is just read... */
                         
      bPktReceived = true;
      newData       -= packetSize;
      m_recvBegin   += packetSize;
                       
      if(newData < 0) {
        newData = 0;
        break;
      }
    } /* while( newData > 0) */
      
    m_recvOfs = newData;
    if ( bPktReceived )
    {
      if(m_recvOfs == 0)
      {                   
        m_recvOfs   = 0;
        m_recvBegin = 0;          
      }
      else if (m_recvBegin + m_recvOfs + socketCommunication_namespace::MAX_PACKET_SIZE >  socketCommunication_namespace::RECV_BUFFER_SIZE)
      {                 
        // we don't want to overrun the buffer - so we copy the leftover bits
        // to the beginning of the receive buffer and start over         
        memcpy(m_recvBuf, &m_recvBuf[m_recvBegin], m_recvOfs);
        m_recvBegin = 0;
      }
    }
    
    m_pMutexReadCondwait->mutex_cond_signal();
    
  } /* while loop for recv() ends here */
  
   
  return 0;
}



void ISocketCommunication::thread_handler(int iThreadIndex)
{
  switch(iThreadIndex)  {
    case 1:
      Socket_ProcessInboundPackets();  
      break;
        
    case 2:
      Socket_HandleOutboundPackets();
      std::cout << std::endl << "Remote host connection lost " << std::endl;  
      break;
    
    default:
      break;
  }
  
  /*if exit from thread by any of the thread means some serious error, hence exit from application */
  //m_pChatEvtCmd->executeChatEvtCmd(chatevtcmd_namespace::eStopApp, 0, 0);
  
}

