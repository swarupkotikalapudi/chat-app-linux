#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

#include<string>


namespace socketHelper
{
  
  bool isNumber(const std::string& s);
  void throwException(int iErrNo);
  int waitOnUserInput(char *pBuf, const int maxBufLength, int *iBufRecvdLen);
  bool portNoExceptionHandler(std::string& sPortNo);
  int networkExceptionHandler(std::string& sNetworkIP);
  

} /* end of namespace */

#endif

