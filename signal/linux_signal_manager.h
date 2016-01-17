#ifndef _LINUX_SIGNAL_MANAGER_H
#define _LINUX_SIGNAL_MANAGER_H

#include<iosfwd>

#include<set>
#include<algorithm>
#include<memory>



class IsignalListner;



/* this class is used by source where some event(e.g. signal) happened,  these event need to be informed to subscriber */
class linuxSignalManager
{
private:

typedef void(*fptrSignalHandler)(int);

fptrSignalHandler         m_SignalHandler;
std::set<std::shared_ptr<IsignalListner>> m_listners;

protected:
  
public:

 linuxSignalManager();
~linuxSignalManager();

 virtual int registerSignalListner(std::shared_ptr<IsignalListner> p_listen);
 virtual int unregisterSignalListner(std::shared_ptr<IsignalListner> p_listen);
  
 virtual void informSignalListner(int sig = 0);

 virtual int registerSignalHandler(fptrSignalHandler sigHandler);
 
};


#endif /* _LINUX_SIGNAL_MANAGER_H */

