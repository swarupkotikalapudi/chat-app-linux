#ifndef _CTEXTPACKET_H
#define _CTEXTPACKET_H

#ifndef _IPACKET_H
#include "IPacket.h"
#endif

#include  <string>

/*This class represent text based packet  */
class cTextPacket:public IPacket
{ 

  cTextPacket(const cTextPacket&);
  cTextPacket& operator=(const cTextPacket&);
  
  protected:
    std::string          m_PacketData;
  public:
  
  cTextPacket();
  cTextPacket(const char* data);
  ~cTextPacket();
  
  const char* packet_data();
  int   packet_size();
  
  int copyPacketData(char* data,  int offsetSize);
};

#endif /* _CTEXTPACKET_H */
