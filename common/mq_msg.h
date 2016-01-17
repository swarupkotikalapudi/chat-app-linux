#ifndef _MQ_MSG_H
#define _MQ_MSG_H

namespace mqmsg_namespace
{
  enum eMQMSG
  {
    eCHILD_PROC_KILLED = 2,
    ePROCESS_CLIENT_CONNECTIONS = 4,
    eSTART_APP = 8,
    eRESTART_APP = 16,
    ePRINT_STATS = 32,
    eQUIT  = 64,
    eSTOP_THREAD = 128,
    ePACKET_RECV_START = 256,
    ePACKET_SEND_START = 512,
    eRESOURCE_CLOSE = 1024,
  
    ePACKET_RECVD = 2048
  
  };
  
  enum  mq_op_mode {
  CREATOR_MODE = 2,
  RECV_MODE = 4,
  SEND_MODE = 8
};

typedef struct sockMsg
{
  unsigned int  msgID;
  unsigned int iMsg;
  const void*        pBuf;
}sockMsg;


}


#endif /* _MQ_MSG_H */
