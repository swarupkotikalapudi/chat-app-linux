#ifndef _CMSGQUEUE_H
#define _CMSGQUEUE_H

#include<mqueue.h>
#include<string>

#ifndef _MQ_MSG_H
#include "mq_msg.h"
#endif




/*This class encapsulate pthread message queue functionality */

class cMsgQueue 
{
  private:
  
  mqd_t m_mqd;
  int   m_flags;
  int   m_mode;
  struct mq_attr m_attr;
  
  int m_op_mode;
  
  std::string m_mq_strName;
  
  cMsgQueue(const cMsgQueue&);
  cMsgQueue& operator=(const cMsgQueue&);  
  
  protected:
    
    
  public:
     cMsgQueue(int eMode);
    ~cMsgQueue();
    
    int mq_Open(const char* mq_name);
    int mq_Close();
    int mq_Send(struct mqmsg_namespace::sockMsg* pMsg);
    int mq_Recv(struct mqmsg_namespace::sockMsg* msg);
    
};


#endif /* _CMSGQUEUE_H */

