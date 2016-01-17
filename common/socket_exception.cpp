#ifndef _SOCKET_EXCEPTION_H
#include "socket_exception.h"
#endif



socketException::socketException(std::string msg) throw() : sErrMsg(msg)
{

}



const char* socketException::what() const  throw()
{
  return sErrMsg.c_str();
}

