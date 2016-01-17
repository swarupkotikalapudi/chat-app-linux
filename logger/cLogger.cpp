#ifndef _C_CLOGGER_H
#include "cLogger.h"
#endif

#include<iostream>
#include<cstdlib>

#include<string>


using namespace std;

extern  const char* __progname;


cLogger*  cLogger::m_pLoggerThisPtr = nullptr;
string cLogger::m_sLogfileName  = __progname;
std::ofstream cLogger::m_LogfileHandle;

void  cLogger::destroycLoggerSingleton()
{
  if(m_pLoggerThisPtr)  {
    delete  m_pLoggerThisPtr;
    m_pLoggerThisPtr = nullptr;
  }
    
}



cLogger* cLogger::getLogger()
{
  
  if(m_pLoggerThisPtr == nullptr)
  {        
    m_pLoggerThisPtr  = new cLogger();
      
    m_sLogfileName  +=  "_log.txt";
    
    m_LogfileHandle.open(m_sLogfileName.c_str(), ios::out | ios::app);
    
  }  
    
  return  m_pLoggerThisPtr;
}



void  cLogger::log_write(const string sLogMsg)
{
  m_listLog.push_back(sLogMsg);
  
  /* Todo : this call ( log_commit_fs() )has to be move to a thread where periodically logs are commited to file system.. */
  log_commit_fs();
  
}



void  cLogger::log_commit_fs()
{
  string tStr;
  
  while( ! m_listLog.empty() )
  {
    tStr = *m_listLog.begin();  
    m_LogfileHandle << endl << tStr << endl;
        
    m_listLog.pop_front();
  }
  
}


