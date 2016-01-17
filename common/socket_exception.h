#ifndef _SOCKET_EXCEPTION_H
#define _SOCKET_EXCEPTION_H

#include<iostream>
#include<exception>


/*This class is used for throwing exception   */
class socketException : virtual public std::exception
{
  
protected:

  std::string sErrMsg;

public:
  socketException() throw() : sErrMsg("Unknown error happened..") {}; 
  socketException(std::string msg) throw();
  virtual ~socketException() throw() {} 
  virtual const char *what() const throw();
}; 

class portNetWorkException : public socketException
{
  public:
    portNetWorkException(std::string msg) : socketException(msg) { }
};

class protocolException : public socketException
{
  public:
    protocolException(std::string msg) : socketException(msg) { }
};


class inputValidationException : public socketException
{
  public:
    inputValidationException(std::string msg) : socketException(msg) { }
};

class objectCreationException : public socketException
{
  public:
    objectCreationException(std::string msg) : socketException(msg) { }
};

#endif /* _SOCKET_EXCEPTION_H */
