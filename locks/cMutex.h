#ifndef _CMUTEX_H
#define _CMUTEX_H

#include<pthread.h>

/* This class represent pthread mutex primitive */
class cMutex
{
private:
  pthread_mutex_t m_mutex;


  cMutex(const cMutex&);
  cMutex& operator=(const cMutex&);
  
  
protected:

public:
  
  cMutex();
  ~cMutex();
  bool lock();
  bool unlock();
  
};



#endif /* _CMUTEX_H */
