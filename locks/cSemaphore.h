#ifndef _CSEMAPHORE_H
#define _CSEMAPHORE_H

#include<semaphore.h>

namespace semaphore_namespace
{
  enum  eSemaphoreType
  {
    eLOCK = 2,  /* It will be used a simple binary semaphore or mutex kind of locK */
    eCONDITION=4 /* iT will be used to specify semaphore as a condition variable */
  };
}

/*This class represent pthread semaphore */
class cSemaphore
{
private:
  sem_t *m_pSem;
  
  cSemaphore(const cSemaphore&);
  cSemaphore& operator=(const cSemaphore&);
  
protected:
 
  
public:

   cSemaphore(semaphore_namespace::eSemaphoreType iSemaphoreType);
  ~cSemaphore();
   bool sem_Wait();
   bool sem_Post();
    
};


#endif /* _CSEMAPHORE_H */
