#ifndef _LINUX_SIGNAL_MANAGER_H
#include "linux_signal_manager.h"
#endif

#ifndef _ISIGNAL_LISTNER_H
#include "Isignal_Listner.h"
#endif

#ifndef _SIGNAL_DEFINATION_H
  #include  "signal_defination.h"
#endif


#include<signal.h>



/* constructor */
linuxSignalManager::linuxSignalManager(void):m_SignalHandler(nullptr)
{
}


/*destrcutor */
linuxSignalManager::~linuxSignalManager(void)
{
}



int linuxSignalManager::registerSignalListner(std::shared_ptr<IsignalListner> p_listener)
{
  if(p_listener == nullptr)
    return -1;
    
  m_listners.insert(p_listener);
 
  return 0;
}



int linuxSignalManager::unregisterSignalListner(std::shared_ptr<IsignalListner> p_listener)
{
  int iRet = -1;
  std::set< std::shared_ptr<IsignalListner> >::const_iterator iterator = m_listners.find(p_listener);
  if(iterator != m_listners.end())
  {
    m_listners.erase(iterator);
    iRet = 0;
  }
  
  return iRet;
}



void linuxSignalManager::informSignalListner(int sig_event)
{
  std::for_each( m_listners.begin(), m_listners.end(), [&](std::shared_ptr<IsignalListner> l) { l->signalEventOccured(sig_event); } );
}



int linuxSignalManager::registerSignalHandler(fptrSignalHandler fptrSigHandler)
{
  /*Caution: Please add here only fatal error meesage from which recovery not possible and application will have to exit.
             Please don't add custom and non fatal error message here.. */  
  signal(signal_namespace::SIG_PROCESS_ABORT, fptrSigHandler );
  
  
  signal(signal_namespace::SIG_INVALID_MEMORY_REFERENCE, fptrSigHandler );
  signal(signal_namespace::SIG_KILL_PROCESS, fptrSigHandler );
  
  signal(signal_namespace::SIG_MEMORY_ACCESS_ERROR, fptrSigHandler );
  signal(signal_namespace::SIG_ARITHEMATIC_ERROR, fptrSigHandler );
  signal(signal_namespace::SIG_ILLEGAL_INSTRUCTION, fptrSigHandler );
  
  
  if(signal(signal_namespace::SIG_PARENT_PROCESS_TERMINATE, fptrSigHandler ) == SIG_IGN )
    signal(signal_namespace::SIG_PARENT_PROCESS_TERMINATE, SIG_IGN); /* SIG_PARENT_PROCESS_TERMINATE => SIG_IGN */ 
  
  if( signal(signal_namespace::SIG_FORCEFUL_TERMINAL_CLOSE, fptrSigHandler )  ==  SIG_IGN )
    signal(signal_namespace::SIG_FORCEFUL_TERMINAL_CLOSE, SIG_IGN); /* SIG_FORCEFUL_TERMINAL_CLOSE  => SIGHUP */
  
  if( signal(signal_namespace::SIG_FORCEFUL_PROCESS_CLOSE, fptrSigHandler ) == SIG_IGN )
    signal(signal_namespace::SIG_FORCEFUL_PROCESS_CLOSE, SIG_IGN);  /* SIG_FORCEFUL_TERMINAL_CLOSE  => SIGQUIT */
    
  return 0;   
}

