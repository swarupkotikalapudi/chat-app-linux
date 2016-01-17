#ifndef _SOCKET_HELPER_H
  #include "socket_helper.h"
#endif
#ifndef _SOCKET_EXCEPTION_H
  #include "socket_exception.h"
#endif
#ifndef _SIGNAL_DEFINATION_H
  #include "signal_defination.h"
#endif
#ifndef _COMMON_DEF_H
  #include "common_def.h"
#endif

#include<iostream>
#include<sys/wait.h>

#include<algorithm>
#include<ctype.h>

#include<sys/socket.h>

#include <errno.h> //errno after a system call

#include <netdb.h> //path name for /etc/services/ e.g. getaddrinfo() errror code etc..

#include<netinet/in.h> /* e.g. 1) endian change function e.g. ntoh() etc.. 2) struct sockaddr_in etc.. */

#include<string.h>
#include <unistd.h>

/* This namespace has helper functions */
namespace socketHelper
{  


  /*this no is based on fact that for IP4 max ip notation is 16 and for IP6 it is 46 including null terminated     character, so just took  the  maximum no */
  const int MAX_HOST_IP_LEN = 46;
  const int MAX_PORT_LEN    = 16;




  bool isNumber(const std::string& s)
  {
    return !s.empty() && std::find_if( s.begin(), s.end(), [](char c) { return ! std::isdigit(c); } ) == s.end();
  }


  void throwException(int iErrNo)
  {  
    switch(iErrNo)
    {
      case  commondef_namespace::eINPUT_PASSED_NOT_SUPPORTED:
        throw inputValidationException("The argument passed are not supported, please refer to release note..") ;
        break;
  
      case  commondef_namespace::eINPUT_VALIDATION_FAILED:
        throw inputValidationException("Input validation failed..");
        break;
  
      case  commondef_namespace::eOBJECT_CREATION_FAILED:
        throw objectCreationException("Object creation failed..");
        break;

      case EADDRINUSE:
              
        throw inputValidationException( strerror(iErrNo) );
        break;
    
      case  EBADF:
      case  EAI_NONAME:
      case EAI_NODATA:
      case  EAI_SERVICE:
          
        throw portNetWorkException("Port no or IP address not supported.");
        break;
    
      case EACCES: /*  permission denied */
        throw portNetWorkException("Port no or IP address not supported.");
        break;
      
      case ECONNREFUSED:
        throw portNetWorkException("Port no or IP address not supported.");
        break;
          
      case  commondef_namespace::ePROTOCOL_NOT_SUPPORTED:
        throw protocolException("Protocol not supported");
        break;
      
      case  ENOMEM:
        throw protocolException( strerror(iErrNo) );           
        break;
        
      case  EAFNOSUPPORT:
        throw protocolException( strerror(iErrNo) );           
        break;
      
      case  EADDRNOTAVAIL:
        throw portNetWorkException( strerror(iErrNo) );
        break;
      
      case  EPROTONOSUPPORT:
        throw portNetWorkException( strerror(iErrNo) );
        break;
    
      case EINVAL:
        throw inputValidationException( strerror(iErrNo) );
        break;
                
      default:
        
        throw (strerror(iErrNo));
    }  
  }


  


  int waitOnUserInput(char *pBuf, const int maxBufLength, int *iBufRecvdLen)
  {
    char *cPtr;
    fd_set readfds;
    int ret;
    /* We can use cin / scanf or fgets etc to get the user input, but this will keep the cpu busy, hence to avoid cpu usage because of main     process or main thread here 'select' system call is used, which will be awake the process only when user gives some input.. */
  
  /* Note: this function doesn't restrict to some maximum buffer size while input is give, it needs to be fixed... */
  
    *iBufRecvdLen = 0;
    errno = 0;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds); /* Wait on stdin for input. */
    ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
                        
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
    
      while(*iBufRecvdLen < maxBufLength)
      {      
        ret = read (STDIN_FILENO, pBuf + *iBufRecvdLen, maxBufLength - *iBufRecvdLen);
        if (ret == -1) {
          if (errno == EINTR) { /* Interrupted hence restart read() */
            continue;
          }
          else  { /* error may have occured */     
            //std::cout << std::endl << "read error occured..";           
            return -1;
          }        
        }
            
        if( (cPtr = (char *) memchr(&pBuf[*iBufRecvdLen], 0x0A, ret) ) )  {
          *iBufRecvdLen += ret;        
          return 0;
        }
      
        *iBufRecvdLen += ret;
        //break;
      }
    
    }
  
   return 0;
  }



  bool portNoExceptionHandler(std::string& sPortNo)
  {
    int iPortNo;
    bool  bPortNoValid = false;
    std::string sTmp;
    
    if (  isNumber(sPortNo) ) {
      iPortNo = atoi( sPortNo.c_str() );
      bPortNoValid = ( ( iPortNo > 1023 && iPortNo <= 65535) ? true : false);
    }
  
    if(  bPortNoValid == false ) {
      std::cout << std::endl << "Please enter port no : " ;
      std::cin.width(MAX_PORT_LEN);
      std::cin >>  sTmp;
      sPortNo = sTmp;
      std::cin.clear();
      std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n');       
    }
      
    return bPortNoValid;
  }



  int networkExceptionHandler(std::string& sNetworkIP)
  {
    /* IP address is not resolved, hence request to input ip address again */        
    std::cout << std::endl << "Please enter IP address or Host name : " ;
    std::cin.width(MAX_HOST_IP_LEN);
    std::cin >>  sNetworkIP;
    std::cin.clear();
    std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
  }


} /* end of namespace */
