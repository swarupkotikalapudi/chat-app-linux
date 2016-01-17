#ifndef _C_CLOGGER_H
#define _C_CLOGGER_H

#include<fstream>
#include<list>

/*This class encapsulate a basic logger functionality */
class cLogger
{

  cLogger() {};
  ~cLogger() {};
  
  cLogger(const cLogger&);  /* copy constructor  */
  cLogger& operator=(const  cLogger&) ; /* assignment operator  */
  
  static  std::string m_sLogfileName;
  static  cLogger* m_pLoggerThisPtr;
  
  static  std::ofstream m_LogfileHandle;
  
  std::list<std::string>  m_listLog;
  
protected:
  
public:

  void  log_write(const std::string sLogMsg);
  
  void  log_commit_fs();
  
  static cLogger* getLogger();
      
  static void destroycLoggerSingleton();    
};


#endif /* _C_CLOGGER_H */

