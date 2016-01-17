#ifndef _CMUTEXCONDWAIT_H
#define _CMUTEXCONDWAIT_H

#include<pthread.h>

/* This class represent pthread mutex conditional wait primitive  */
class cMutexCondwait
{
private:
  pthread_mutex_t m_mutex;
  pthread_cond_t  m_cond_var;

  cMutexCondwait(const cMutexCondwait&);
  cMutexCondwait& operator=(const cMutexCondwait&);
  
  
protected:

public:
  
  cMutexCondwait();
  ~cMutexCondwait();
  bool lock();
  bool unlock();
  int   mutex_cond_wait();
  int   mutex_cond_signal();
  
};



#endif /* _CMUTEX_H */
