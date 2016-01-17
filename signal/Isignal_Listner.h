#ifndef _ISIGNAL_LISTNER_H
#define _ISIGNAL_LISTNER_H

/* This class will be used by subscriber who are interested to be told about some events.. */
class IsignalListner
{
protected:

public:

  virtual void signalEventOccured(int sig_event) = 0;
   
  IsignalListner();
  ~IsignalListner();
   
};


#endif /* _ISIGNAL_LISTNER_H */

