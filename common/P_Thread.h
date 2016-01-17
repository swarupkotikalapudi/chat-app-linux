#ifndef _P_THREAD_H
#define _P_THREAD_H

#include<pthread.h>
#include<memory>

class cSemaphore;

namespace pthread_namespace
{
  enum  eThreadState
  {
    eJOIN,
    eDETACH
  };
}

extern "C" void* thread_catch(void* arg);



class P_Thread
{
private:
  pthread_t   *m_pHandle;
  
  std::unique_ptr<cSemaphore> m_pSemaphore;
  
  int         m_iMaxThreadCreated;
  int         m_iThreadIndx;
  
  
  
  P_Thread(const P_Thread&);
  P_Thread& operator=(const P_Thread&);
  
  friend   void* thread_catch(void* arg);
  
protected:
  
   
public:
  P_Thread();
  ~P_Thread();
  
  int          P_setThreadSupportNeeded(int iMaxThreadCreateRequest);
   
  int          threadStart(pthread_namespace::eThreadState eThreadState);
  virtual void thread_handler(int iThreadIndx);
  void         wait_for_exit(int iThreadIndx);
    
  int          thread_cancel(int iThreadIndx);    
};

#endif /* _P_THREAD_H */

