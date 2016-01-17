#ifndef _CSHAREDMEM_H
#define _CSHAREDMEM_H

#include <fcntl.h>    /* Defines O_* constants */

/*This class is used to encapsulate posix shared memory implemnetaion */
class cSharedMem
{
  int     m_iClientConnectionSupported;
  int     m_fd;
  size_t  m_sharedMemSize;
  const char*   m_pSharedMemoryName;
  void*   m_pShareStartAddress; /* stores the starting address of shared memory address */
  
  int*    m_pClientRecdPtr; /* used to cache the client process id's address in shared memory */
  
  cSharedMem(const cSharedMem&);
  cSharedMem& operator=(const cSharedMem&);
  
protected:

public:
  cSharedMem(int  iClientConnCount, const char* pSharedMemName);
  ~cSharedMem();
  
  int sharedMem_Open();
  int sharedMem_Close();
  
  int sharedMem_UpdateClient(pid_t iProcessID, unsigned int iTx, unsigned int iRx);
  int sharedMem_addClient(pid_t iProcessID);
  int sharedMem_removeClient(pid_t iProcessID);
  int sharedMem_removeAllClient();
  int sharedMem_printClientStats();
  
};


#endif
