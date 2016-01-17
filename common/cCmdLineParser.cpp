#ifndef _C_CMDLINEPARSER_H
#include "cCmdLineParser.h"
#endif
#include<iostream>
#include<sys/socket.h>
#include<string.h>

#include<string>
#include<ctype.h>
#include<getopt.h>


/* This is the command line option supported by this chat server application */
static char cmdopts[] = "hHs:p:t:u:n:S:P:T:U:N:";

cCmdLineParser::cCmdLineParser() : m_iSocketType(-1), m_iNetworkDomain(-1), m_sIPAddr(""), m_sPortNo("")
{
  prepareRules();
}



cCmdLineParser::~cCmdLineParser()
{
}



void cCmdLineParser::prepareRules()
{
  /*Todo: these rules are currently hardcoded, need to change it to read from configuration files and then store it */
  
  /*This rule specific to transport protocol, as currently only TCP is supported, so TCP entry only is being added */
  protoMap.insert( std::make_pair('t', SOCK_STREAM) );
  //protoMap.insert( std::make_pair('u', SOCK_DGRAM) );
  
  /*This rule specific to network protocol e.g. IP4, IP6 and Unix domain socket (local) */
  networkMap.insert( std::make_pair("ip4", AF_INET) );
  networkMap.insert( std::make_pair("ip6", AF_INET6) );
  networkMap.insert( std::make_pair("un", AF_UNIX) );
}



void cCmdLineParser::usage()
{
	std::cout << std::endl << std::endl << "usage: -s server IP or Host Name -p  portno or service type -t transport protocol(TCP or UDP) ";
	std::cout << std::endl << "Example 1 --> ./my_server -s localhost -p 1234 -t T -n IP4 ";
	std::cout << std::endl << "Example 2 --> ./my_server -s 127.0.0.1 -p 1234 -t T -n IP4";
	std::cout << std::endl << std::endl << "	-s a.b.c.d or xyz.com (IP address or server name ) \n";
	std::cout << std::endl << "	-p xxxx	( Port number to be used ) \n";
	std::cout << std::endl << "	-t or -u (transport protocol TCP or UDP ) \n";
	std::cout << std::endl << "	-n ip4 or ip6 or un ( Network addressing IP4 or Ip6 or Unix domian) \n";
}



int cCmdLineParser::parse_verify_encode_args(int argc, char* argv[])
{
  int optch, iRet = 0;
  std::string  sNetworkDomain;
   
  while (  (optch = getopt(argc,argv,cmdopts)) != -1 )
  {
    switch ( optch ) {
      case 'h': /* -h for help */
        usage();
        exit(0);
        
      case 's' : /* IP address or host name */
      case 'S' :
        //strncpy(m_sIPAddr, optarg, MAX_HOST_IP_LEN);
        m_sIPAddr = optarg;
        
        break;
        
      case 'p' :  /* -p port no or service type */
      case 'P' :
        //strncpy(m_sPortNo, optarg, MAX_PORT_LEN);
        m_sPortNo = optarg;
        break;

      case 't' :  /* transport type -- TCP */
      case 'T' :
        if( encodeSocketType(*optarg) )  {
          std::cout << std::endl << "Socket type please enter -t T for TCP..  "  << std::endl;
          exit(0);
        }
        break;
  
      case 'u' :  /* transport type -- UDP */
      case 'U' :
        std::cout << std::endl << "Socket type please enter -t T for TCP..  "  << std::endl;
        exit(0);
        break;
        
      case 'n' :
      case 'N' :        
        sNetworkDomain = optarg;
        if( encodeNetworkDomain(sNetworkDomain) )  {
          std::cout << std::endl << "Please enter -n ip4 (IP v4) or -n ip6 (IP v6) or -n un (Unix socket) for correct networking layer.."  << std::endl;
          exit(0);
        }
        break;
          
      default :
        /* Option error */
        usage();
        exit(0);
    }
 }
 
  if(m_iSocketType == -1) {
    std::cout << std::endl << "Socket type please enter -t T for TCP..  "  << std::endl;
    exit(0);
  }
  
  if(m_iNetworkDomain == -1) {
    std::cout << std::endl << "Please enter -n ip4 (IP v4) or -n ip6 (IP v6) or -n un (Unix socket) for correct networking layer"  << std::endl;
    exit(0);
  }
  
  if(m_sPortNo[0] == 0 || m_sPortNo[0] == '0' ) 
  {
    std::cout << std::endl << "Please enter -p port no, a numeric value 'greater than 1023'. "  << std::endl;
    exit(0);
  }
 
 return iRet;
}



int cCmdLineParser::encodeSocketType(char c)
{
  int iRet = -1;
  std::map<char, int>::iterator itProto;
  
  c       = tolower(c);
  itProto = protoMap.find( c );
  if(itProto != protoMap.end())  {
    m_iSocketType = (*itProto).second;
    iRet = 0;
  }
  
  return iRet;
}




int cCmdLineParser::encodeNetworkDomain(std::string& sIpaddr)
{
  int iRet = -1;
  std::map<std::string, int>::iterator itNetwork;
 
  std::transform(sIpaddr.begin(), sIpaddr.end(), sIpaddr.begin(), ::tolower );
  
  itNetwork = networkMap.find(sIpaddr);
  if(itNetwork != networkMap.end())  {
    m_iNetworkDomain  =  (*itNetwork).second;
    iRet          =  0;
  }
  
  return iRet;
}



const char* cCmdLineParser::getIPAddress_string()
{
  return m_sIPAddr.c_str();
}

const char* cCmdLineParser::getPortNo_string()
{
  return m_sPortNo.c_str();
}

const char* cCmdLineParser::getSocketType_string()
{
  if(m_iSocketType == SOCK_STREAM)
    return "TCP";
  else if (m_iSocketType == SOCK_DGRAM)
    return "UDP";
  else
    return "Transport protocol not supported";
}

const char* cCmdLineParser::getNetworkDomain_string()
{
if(m_iNetworkDomain == AF_INET)
  return "IP4";
else if(m_iNetworkDomain == AF_INET6)
  return "IP6";
else if(m_iNetworkDomain == AF_UNIX)
  return "Unix Domain";
else
  return "Network type not supported";  

}



int cCmdLineParser::getSocketType_number()
{
  return m_iSocketType;
}



int cCmdLineParser::getNetworkDomain_number()
{
  return m_iNetworkDomain;
}



int cCmdLineParser::setPortNo_string(std::string& sPortNo)
{    
  m_sPortNo = sPortNo;
  return 0;
}



int cCmdLineParser::setIPAddress_string(std::string& sIPAddr)
{  
  m_sIPAddr = sIPAddr;
  return 0;
}

