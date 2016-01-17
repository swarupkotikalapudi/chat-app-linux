#ifndef _CSHAREDMEM_H
#include "cSharedMem.h"
#endif

#include<iostream>
#include <sys/mman.h>

#include<string.h>
#include <unistd.h>

/*
  Note: This class is based on posix shared memory, which is shared between different processes.
        It is used by server application to store each client connections's rx/tx bytes from server's perspective.
        
        The constructor of this class is passed no of client connection supported, based on  that it resevere the shared memory.
        The memory layout is like this: a)First 4 or 8 bytes (depend on 32/64 bit) count of client process connected.
                                        b) After first 4 or 8 bytes, each 4 or 8 bytes * 3 is reserved for each client connection.
                                           The first one stores client process id, next two entries stores tx and rx counter for 
                                           that particular client process id..
*/

/*Constructor */
cSharedMem::cSharedMem(int iClientConnCount, const char* pSharedMemName): m_fd(-1),m_pSharedMemoryName(pSharedMemName),m_pShareStartAddress(nullptr), m_pClientRecdPtr(nullptr)
{
  m_iClientConnectionSupported = ( iClientConnCount > 0 ? iClientConnCount : 0);  
  m_sharedMemSize = sizeof(int *) + (iClientConnCount * ( sizeof(int *) * 3 ) );
}


/*destructor */
cSharedMem::~cSharedMem()
{
  if(m_fd > 0)  {
    munmap(m_pShareStartAddress, m_sharedMemSize);
    
    shm_unlink( m_pSharedMemoryName );
    m_fd = 0;
    m_pShareStartAddress  = nullptr;
  }
}



/*shared memory open */
int cSharedMem::sharedMem_Open()
{
  int flags     = O_RDWR | O_CREAT;
  mode_t perms  = S_IRUSR | S_IWUSR;
  
  if(m_fd > 0)  {
    shm_unlink( m_pSharedMemoryName );
      
    m_fd = -1;
  }
  
  if( ( m_fd = shm_open(m_pSharedMemoryName, flags, perms) ) == -1)  {    
    
    perror("\n shm_open");
    return -1;
  }
  
  if (ftruncate(m_fd, m_sharedMemSize) == -1)  {    
    return -1;
  }
  
  /* Map shared memory object */
  m_pShareStartAddress = mmap(NULL, m_sharedMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
  if (m_pShareStartAddress == MAP_FAILED) {   
    return -1;
  }
  
  memset(m_pShareStartAddress, 0, m_sharedMemSize );
  
  close(m_fd);
  
  return 0;
}


/*Shared memory close */
int cSharedMem::sharedMem_Close()
{
  if(m_fd > 0)  {
    munmap(m_pShareStartAddress, m_sharedMemSize);
    
    shm_unlink( m_pSharedMemoryName ) ;
    m_fd = 0;
    m_pShareStartAddress  = nullptr;
  }
   
  return 0;
}



/*Shared memory update client tx/rx information */
int cSharedMem::sharedMem_UpdateClient(pid_t iProcessID, unsigned int iTx, unsigned int iRx)
{
  int* ptr = (int *) m_pShareStartAddress, i = m_iClientConnectionSupported, iRet = -1;
  bool bFound = false;
  
  if( ! m_pShareStartAddress )  { 
    return -1; /*If shared memory is not initialize, exit */
  }
    
  if(m_pClientRecdPtr && (*m_pClientRecdPtr == (int) iProcessID) ) { /*If the process id of the client is already cached */
    ptr     = ++m_pClientRecdPtr;
    bFound  = true;
  }
  else  { /* Entry of client's process id related entry not known, hence iterate to find */
    ptr++; /* move to client related first record  */
    while(i)  {
      if( *ptr == (int) iProcessID  )  {
        bFound  = true;            /* client process id entry found in shared memory */   
        m_pClientRecdPtr  = ptr++; /* cache the client's location pointer for subsequent updates */     
        break;
      }
      i--;
      ptr = ptr + 3; /* move to next client process related entry */
    }
  }
  
  if(bFound  == true) { /* As client process id entry found, incrment tx/rx counter */
  
    /* Todo -- In computer architecture everything is finite, addition can cross the limit for integer max value,
           code need to write to take care of integer overflow and subsequent additions.. */
    *ptr++ += iTx;
    *ptr   += iRx;
    iRet   = 0;
  }
       
  return iRet;
}



/*Acquire the slot for client connection related information */
int cSharedMem::sharedMem_addClient(pid_t iProcessID)
{
  int i    = m_iClientConnectionSupported, iRet = -1;
  
  
  /* First four/eight byte(depedning upon 32 bit or 64 bit computer architecture) will have count of number of clients currently active.. */  
  int* ptr = (int *) m_pShareStartAddress;
  
  if(! m_pShareStartAddress )  {
    std::cout << "sharedMem_addClient failed.." << std::endl;
    return -1;
  }
  
  /*
    Todo: the addition of new client is done based on whereever space/slot is free. Hence the information is not kind of sorted based on oldest client connection to new client connection.
    A auxiliary data structure is needed so that a) printing of client information from oldest to newsest client connection
                                                 b) Adding new client process id is o(n) in worst case
                                                 c) Update is o(n) first time, subsequently o(1)
  */
     
  if(*ptr == m_iClientConnectionSupported)  {
    /*already maximum client is supported, no more new client can be supported */
    errno = EACCES;
    return -1;
  }
  else  {
    ++*ptr++; /* a)increment client count b) then move to client related first record */
  }
  
  while(i)  {
    if(! *ptr )  { /* slot is empty, hence in the first slot put the process id, remaining two slot will have tx/rx related record */
      *ptr = (int) iProcessID;
      memset(ptr + 1, 0, sizeof(int *) * 2);
      iRet = 0;            
      break;
    }
    
    ptr = ptr + 3;
    i--;
  }
   
  return iRet;
}



/*acquire the resource for freed client */
int cSharedMem::sharedMem_removeClient(pid_t iProcessID)
{
  int* ptr = (int *) m_pShareStartAddress, i = m_iClientConnectionSupported, iRet = -1;
  bool bFound = false;
  
  if(! m_pShareStartAddress)  {
    return -1;
  }
  
  
  if(m_pClientRecdPtr && (*m_pClientRecdPtr == (int) iProcessID) ) { /*If the process id of the client is already cached */    
    ptr = (int *) m_pClientRecdPtr;
    bFound  = true;
  }
  else  { /* Entry of client's process id related entry not known, hence iterate to find */
    ptr++; /* move to client related first record  */
    while(i)  {
      if( *ptr == (int) iProcessID  )  {
        bFound  = true;
        break;
      }
      i--;
      ptr = ptr + 3; /* move to next client process related entry */
    }
  }
  
  if(bFound)  {
    //*ptr++ = 0; /* process id is set to zero */
    //*ptr++ = 0; /* tx count set to zero */
    //*ptr   = 0; /* rx count set to zero */
    memset(ptr, 0, sizeof(int *) * 3);
     (*(int *) m_pShareStartAddress)--;
    
  }
    
  return iRet;
}



int cSharedMem::sharedMem_removeAllClient()
{
  int iRet = -1;
  if(m_pShareStartAddress)  {
    int* ptr  = (int *) m_pShareStartAddress;
    memset(ptr, 0, m_sharedMemSize );
    iRet = 0;
  }
  
  return iRet;
}


/*Print the stats about client connection.. */
int cSharedMem::sharedMem_printClientStats()
{  
  int* ptr = (int *) m_pShareStartAddress;
  int i = m_iClientConnectionSupported;
  int j = 1;
  
  if(!m_pShareStartAddress)
    return  -1;
    
  if(*ptr  <= 0)  {
    std::cout << std::endl  <<  "There is no client connected. " << std::endl;
    return 0;
  }
  
  ptr++; /* move to client related first record */
  
  while(i)  {
    if(*ptr)  {
      std::cout  << "--" ;
      std::cout  << std::endl << "Client Connection [ " << j++ << " ] : " << *ptr++ << std::endl;
      std::cout  << "Byte sent (Tx)     : "     << ( (*ptr++) >> 3 )   << std::endl; /* divide by 8 to get bytes */
      std::cout  << "Byte Received (Rx) : " << (*ptr >> 3)     << std::endl;        /* divide by 8 to get bytes */ 
      ptr++; /* move to next process id */
    }
    else  {
      ptr = ptr + 3; /* move to next process id */      
    }
    
    i--;
  }
    
  return 0;
}

