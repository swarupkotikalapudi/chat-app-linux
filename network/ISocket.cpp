#ifndef _ISOCKET_H
#include "ISocket.h"
#endif
#ifndef _ISOCKET_COMMUNICATION_H
  #include "ISocket_Communication.h"
#endif
#ifndef _ICHATEVTCOMMAND_H
#include "IChatEvtCmd.h"
#endif

#ifndef _SIGNAL_DEFINATION_H
  #include  "signal_defination.h"
#endif

#include<stdlib.h>
#include <unistd.h>

/* This is a default constructor */
ISocket::ISocket(): m_iProcessID(getpid()),m_pSocketCommunication(nullptr),m_iConnectionType(commondef_namespace::eNONE)
{
  /* The application can be either client or server, if user doesn't specify this failing the construction of object */
  if(m_iConnectionType == commondef_namespace::eNONE)
    throw "Socket type client or server need to be specified.";
}



/* This is a constructor with argument */
ISocket::ISocket(commondef_namespace::eConnectionType iType):m_iProcessID(getpid()),m_iConnectionType(iType)
{
  /* The application can be either client or server, if user doesn't specify failing the construction of object */
  if(m_iConnectionType != commondef_namespace::eSERVER && m_iConnectionType != commondef_namespace::eCLIENT)
    throw "Socket type client or server need to be specified.";
}



/* This is a destructor */
ISocket::~ISocket()
{
  
  m_iConnectionType      = commondef_namespace::eNONE;
  m_iProcessID           = -1;
}



void  ISocket::setSocketCommunication(std::unique_ptr<ISocketCommunication> pSocketCommunication)
{
  m_pSocketCommunication = std::move(pSocketCommunication);
}



int   ISocket::appShutdown(void)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->appShutdown();
}


int   ISocket::Socket_Erase_Child_Process_Details(pid_t pid)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_Erase_Child_Process_Details(pid);
}


int ISocket::Socket_Init(void)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_Init(); 
}



int ISocket::Socket_Process_Clients(void)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_Process_Clients();
}



void ISocket::Socket_printConnectionStats()
{
  if(!  m_pSocketCommunication)
    return;
    
  return  m_pSocketCommunication->Socket_printConnectionStats();
}



int ISocket::Socket_doPacketRecv()
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_doPacketRecv();
}



int ISocket::Socket_doPacketSend(const char *buffer, const int bufLen)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_doPacketSend(buffer, bufLen);
}



int ISocket::Socket_Close(void)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_Close();
}



int     ISocket::Socket_Shutdown(void)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return m_pSocketCommunication->Socket_Shutdown();
}



void ISocket::signalEventOccured(int sig_event)
{
  switch(sig_event)
  {
    case signal_namespace::SIG_PROCESS_ABORT:
    case signal_namespace::SIG_MEMORY_ACCESS_ERROR:
    case signal_namespace::SIG_ARITHEMATIC_ERROR:
    case signal_namespace::SIG_ILLEGAL_INSTRUCTION:
    case signal_namespace::SIG_INVALID_MEMORY_REFERENCE:
      exit(0);
      break;
    
    case  signal_namespace::SIG_PARENT_PROCESS_TERMINATE: /* This message is SIG_IGN when ctrl + c is pressed , controlled at application layer hence ignore */
    case  signal_namespace::SIG_FORCEFUL_PROCESS_CLOSE:
    case  signal_namespace::SIG_FORCEFUL_TERMINAL_CLOSE:
   
      break;
    
    case  signal_namespace::SIG_CHILD_PROCESS_CLOSE:
      /*Note : Do nothing, as it is handled in client of this class. */
      break;
        
    case signal_namespace::SIG_KILL_PROCESS:
      if( m_iProcessID == getpid() )
      {
        //std::cout << std::endl << "Server is forcefully killed.." << std::endl;
        appShutdown();
      }
      exit(EXIT_SUCCESS);
      break;
  }
  
}



int ISocket::Socket_attachCommandObject(std::shared_ptr<IChatEvtCmd> pEvtCmd)
{
  if(!  m_pSocketCommunication)
    return  -1;
    
  return  m_pSocketCommunication->Socket_attachCommandObject(pEvtCmd); 
}


