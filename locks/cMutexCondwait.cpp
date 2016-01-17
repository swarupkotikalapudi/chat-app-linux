#ifndef _CMUTEXCONDWAIT_H
#include "cMutexCondwait.h"
#endif
#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif
#include<string.h>

cMutexCondwait::cMutexCondwait()
{
  pthread_mutexattr_t mtxAttr;
  
  errno = 0;
  
  while(1)
  {
    if(pthread_mutexattr_init(&mtxAttr) != 0)
      break;
      
    if( pthread_mutexattr_settype(&mtxAttr, PTHREAD_MUTEX_ERRORCHECK) != 0)
      break;  
    
    if( pthread_mutex_init(&m_mutex, &mtxAttr) != 0)
      break;
      
    if( pthread_mutexattr_destroy(&mtxAttr) != 0)
      break;
  
    pthread_cond_init(&m_cond_var, NULL);
    
    break;
  }
  
  if(errno)
    throw objectCreationException(strerror(errno));
    
}



cMutexCondwait::~cMutexCondwait()
{
  pthread_mutex_destroy(&m_mutex);  
}



bool cMutexCondwait::lock()
{
  if( pthread_mutex_lock(&m_mutex) != 0)  {
    return false;
  }
  return true;
}



bool cMutexCondwait::unlock()
{
  if( pthread_mutex_unlock(&m_mutex) != 0)  {    
    return false;
  }
  return true;
}



int cMutexCondwait::mutex_cond_wait()
{
  return pthread_cond_wait(&m_cond_var, &m_mutex);
}


int cMutexCondwait::mutex_cond_signal()
{
  return pthread_cond_signal(&m_cond_var);
}

