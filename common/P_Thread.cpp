#ifndef _P_THREAD_H
#include "P_Thread.h"
#endif
#ifndef _CSEMAPHORE_H
#include "cSemaphore.h"
#endif

#include<errno.h>



extern "C"
{
  /* callback function when thread is created */
  void* thread_catch(void* arg)
  {
    P_Thread* pThread = static_cast<P_Thread*>(arg);
    
    pThread->m_pSemaphore->sem_Post();
    
    pThread->m_iThreadIndx++; 
    
    pThread->thread_handler(pThread->m_iThreadIndx);
        
    return 0;
  }
}


/*P_Thread constructor */
P_Thread::P_Thread()
{
  m_iThreadIndx = -1;
  m_iMaxThreadCreated = 0;
}


/*P_Thread destructor */
P_Thread::~P_Thread()
{  
  if( m_pHandle ) 
  {        
    delete[]  m_pHandle;
    m_pHandle = nullptr;    
    m_iMaxThreadCreated = 0;
  }
  
}


/*P_Thread initalizer */
int  P_Thread::P_setThreadSupportNeeded(int iMaxThreadCreateRequest)
{
  int i;
  
  if(m_iMaxThreadCreated) {
    errno = EDQUOT; /* Quota exceed  */
    return -1;
  }
    
  /*Member variable initialization */
  m_iThreadIndx = 0;
   
  m_pSemaphore =   std::unique_ptr<cSemaphore>(new cSemaphore(semaphore_namespace::eCONDITION) ); /* Use semaphore for condition wait/post scenario */
  
  m_iMaxThreadCreated = (iMaxThreadCreateRequest > 0 ?  iMaxThreadCreateRequest : 1);
  m_pHandle     = new pthread_t[m_iMaxThreadCreated];
  
  for(i = m_iMaxThreadCreated; i ; i--)  {
    m_pHandle[i - 1] = 0;
  }
      
  return 0;
}


/*P_Thread -- call this function to lauch a new thread */
int P_Thread::threadStart(pthread_namespace::eThreadState e_ThreadState)
{
  int res, iThreadState;
  pthread_attr_t  attr;
  
  errno = 0 ;
    
  if( (m_iThreadIndx + 1) > m_iMaxThreadCreated) {
    //std::cout << std::endl << "threadStart exit due to trying to create more thread.." << std::endl;
    return -1;
  }
  
  if(pthread_attr_init(&attr) != 0)  {
    return -1;
  }
  
  /*By default threads are joinable, if user want change it to detach */
  iThreadState = ( e_ThreadState == pthread_namespace::eDETACH ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
  pthread_attr_setdetachstate(&attr, iThreadState);
  
  /*Todo -- some paparmeter e.g. stack size , thread priority etc are not being set. */
  
  res  = pthread_create(&m_pHandle[m_iThreadIndx],&attr,thread_catch, this);
  
  pthread_attr_destroy(&attr);
  
  if (res != 0) {
    return -1;
  }
  
  /* Basic thread creation succeed, wait from thread's function  semaphore before exiting this function */
  m_pSemaphore->sem_Wait();
  
  return 0;
}



void P_Thread::thread_handler(int iThreadIndex)
{

}


/* Used by jonable thread */
void P_Thread::wait_for_exit(int iThreadIndx)
{ 
  int res; 
  
  errno = 0 ;
  if( ( (iThreadIndx + 1) > m_iMaxThreadCreated ) || ( m_pHandle[iThreadIndx] ) == 0 )  {
    //std::cout << std::endl << "exiting becuase invalid argument passed.. --> exit" << std::endl;
    return;
  }
  
  res = pthread_join(m_pHandle[iThreadIndx], NULL);
  if (res != 0) {
  }
  
}


/* Used to cancel the thread */
int P_Thread::thread_cancel(int iThreadIndx)
{
  int res;
    
  if( ( (iThreadIndx + 1) > m_iMaxThreadCreated ) || ( m_pHandle[iThreadIndx] ) == 0 )  {
    return -1;
  }
  
  res = pthread_cancel(m_pHandle[iThreadIndx]);
  m_pHandle[iThreadIndx] = 0;
  
  return res;
}

