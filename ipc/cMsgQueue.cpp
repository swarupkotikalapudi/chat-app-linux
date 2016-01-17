#ifndef _CMSGQUEUE_H
#include "cMsgQueue.h"
#endif

#include <fcntl.h> /* O_WRONLY etc.. */
#include <sys/stat.h>

namespace
{
  /*As application doesn't assume to be run from root privilage, hence using the default max message constant specified by linux system */
  const int MAX_MSG = 8;
}



cMsgQueue::cMsgQueue(int eMode)
{
  m_mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    
  if(eMode & mqmsg_namespace::CREATOR_MODE) {
    m_flags = O_CREAT | O_RDWR;
  }
  else  {
    m_flags = O_RDONLY;
  }
  
  m_attr.mq_maxmsg    = MAX_MSG;
  m_attr.mq_msgsize   = sizeof(struct mqmsg_namespace::sockMsg);
  m_attr.mq_flags     = 0;
  m_attr.mq_curmsgs   = 0;
  
  m_mqd = (mqd_t) 0;
  
  m_op_mode = eMode;
}



cMsgQueue::~cMsgQueue()
{
  mq_close(m_mqd);
  mq_unlink(m_mq_strName.c_str());
}



int cMsgQueue::mq_Open(const char* mq_name)
{
  
  if( m_op_mode & mqmsg_namespace::CREATOR_MODE)  {  
    mq_unlink(mq_name);
    errno = 0;
    m_mqd = mq_open(mq_name, m_flags, m_mode, &m_attr);
    m_mq_strName = mq_name;
  }
  else  {
    errno = 0;
    m_mqd = mq_open(mq_name, m_flags);
  }
  
  if ( m_mqd == (mqd_t) -1 )  {             
    return -1;
  }
  
  return 0;
}


int cMsgQueue::mq_Close()
{
  int i;
  errno = 0;
  i = mq_close(m_mqd);  
  return i;
}


int cMsgQueue::mq_Send(struct mqmsg_namespace::sockMsg* pMsg)
{    
  if( mq_send(m_mqd, (char *) pMsg, sizeof(struct mqmsg_namespace::sockMsg), 0 ) == -1 ) {
    return -1;
  }
  
  return 0;
}



int cMsgQueue::mq_Recv(struct mqmsg_namespace::sockMsg* msg)
{
  unsigned int prio;
  
  if( mq_receive(m_mqd,(char *) msg, sizeof(struct mqmsg_namespace::sockMsg), &prio ) == -1) {
    return -1;
  }

  return 0;
}

