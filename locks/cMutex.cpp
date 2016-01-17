#ifndef _CMUTEX_H
#include "cMutex.h"
#endif
#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif
#include<string.h>



cMutex::cMutex():m_mutex(PTHREAD_MUTEX_INITIALIZER)
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
  }     
  
  if(errno)
    throw objectCreationException(strerror(errno));
      
}



cMutex::~cMutex()
{
  pthread_mutex_destroy(&m_mutex);  
}


bool cMutex::lock()
{
  if( pthread_mutex_lock(&m_mutex) != 0)  {
    return false;
  }
  
  return true;
}



bool cMutex::unlock()
{
  if( pthread_mutex_unlock(&m_mutex) != 0)  {
    return false;
  }
  return true;
}

