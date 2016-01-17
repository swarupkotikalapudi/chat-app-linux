#ifndef _CTEXTPACKET_H
#include "cTextPacket.h"
#endif

#include<ostream>


cTextPacket::cTextPacket(const char *data)
{
  m_PacketData = data;  
}



cTextPacket::cTextPacket():m_PacketData("")
{
}



cTextPacket::~cTextPacket() 
{
}



int cTextPacket::copyPacketData(char* data, int offsetSize)
{
  if(m_PacketData.size() == 0)
    m_PacketData  = data;
  else
    m_PacketData.insert(offsetSize, data);
 
  return 0;
}



const char* cTextPacket::packet_data()
{  
  return  m_PacketData.c_str();
}



int cTextPacket::packet_size()
{
  return m_PacketData.size();
  return 0;
}

