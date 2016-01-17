#ifndef _SIGNAL_DEFINATION_H
#define _SIGNAL_DEFINATION_H

#include<signal.h>

namespace signal_namespace
  {
    /*user friendly defination  for some signals  */
    enum eSignalNames
    {
      SIG_PROCESS_ABORT         =             SIGABRT,  //the prgram itself called abort to quit the program
      SIG_PROCESS_ABORT1        =           SIGIOT,   //on Linux system, this is similar to SIGABRT

      SIG_CHILD_PROCESS_CLOSE   =      SIGCHLD,  //Parent receive this signal when child process is killed
      SIG_CHILD_PROCESS_CLOSE1  =   SIGCLD,  //Parent receive this signal when child process is killed

      SIG_MEMORY_ACCESS_ERROR   =    SIGBUS,   //this error is generated when program tries to access un-authorized memory address/region
      SIG_ARITHEMATIC_ERROR     =    SIGFPE,  //this signal is generated when airthmetic error like divide by zero happens
      SIG_ILLEGAL_INSTRUCTION   =    SIGILL,   //this signal is generated when process execute illegal instruction

      SIG_PARENT_PROCESS_TERMINATE  =  SIGINT,   //forground process on terminal is closed by ctrl + c
      SIG_FORCEFUL_TERMINAL_CLOSE   = SIGHUP,  //user forcefully closes the terminal itself
      SIG_FORCEFUL_PROCESS_CLOSE    = SIGQUIT, //user presses ctrl \ to terminate process

      SIG_WRITE_ERROR               = SIGPIPE, //this is useful in IPC, when write fails because peer connection closed it read end...
      SIG_INVALID_MEMORY_REFERENCE  = SIGSEGV, //process tries to access invalid memory access

      SIG_KILL_PROCESS              = SIGTERM  //process recevie this signal if kill command is used to terminate the process

    };
  
  } /*namespace ends */

#endif /* _SIGNAL_DEFINATION_H */
