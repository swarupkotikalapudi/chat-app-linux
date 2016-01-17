#ifndef _IPACKET_H
#define _IPACKET_H


/*This class represent basic packet interface */
class IPacket
{

  IPacket(const IPacket&);
  IPacket& operator=(const IPacket&);

protected:
  

public:
  
  IPacket();
  virtual ~IPacket();
    
  virtual const char* packet_data()   = 0;
  virtual int   packet_size()   = 0;

};


#endif /* _IPACKET_H */

