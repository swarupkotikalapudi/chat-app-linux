#ifndef _CSEMAPHORE_H
#include "cSemaphore.h"
#endif
#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif

#include<pthread.h>
#include<sys/stat.h>
#include<fcntl.h>

#include<string.h>

#include <errno.h> //errno after a system call


//using namespace std;


cSemaphore::cSemaphore(semaphore_namespace::eSemaphoreType iSemaphoreType)
{ 
  int iSemValue = 0; /* For condition based semaphore 0 value is used */
  if(iSemaphoreType == semaphore_namespace::eLOCK)
    iSemValue = 1;
  
  m_pSem  = new sem_t();
    
  /*Note: We pass the third argument to identify semaphore is a)binary b) condition variable 
     The value of 0 passed can be use for conditional semaphore
     The value of 1 passed can be use for binary semaphore
     
     Posix semaphore are of two types a)named b)unamed
     This class represent the unnamed semaphore, however can be shared between threads and related processes (process created via fork)
     
     -- Semaphore has some more type e.g. counting and read/writer semaphore these kind of semaphore currently not supported..
  */
  errno = 0;
  if (sem_init(m_pSem, 1, iSemValue) == -1) {
    throw objectCreationException(strerror(errno));
    return;
  }
  
}



cSemaphore::~cSemaphore()
{
  if( m_pSem )  {
    
    sem_destroy( m_pSem );
    
    delete m_pSem;
    m_pSem = nullptr;
  }
 
}


/* block or lock on critcal section */
bool cSemaphore::sem_Wait() 
{        
  if (sem_wait(m_pSem) == -1)  {    
    return false;
  }
  
  return true;
}


/* unblock or unlock on critcal section*/
bool cSemaphore::sem_Post() 
{
  if (sem_post(m_pSem) == -1)  {    
    return false;
  }
  
  return true;
}


