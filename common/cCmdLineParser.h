#ifndef _C_CMDLINEPARSER_H
#define _C_CMDLINEPARSER_H

#include<map>
#include<algorithm>

/*A class which is used to encapsulated command line parsing */
class cCmdLineParser
{

  cCmdLineParser(const cCmdLineParser&);
  cCmdLineParser& operator=(const cCmdLineParser&);
  void  prepareRules();
  
  int encodeSocketType(char c);  
  int encodeNetworkDomain(std::string& spaddr);
    
  std::map<char,int> protoMap;
  std::map<std::string,int> networkMap;
  
  int             m_iSocketType;
  int             m_iNetworkDomain;
  std::string          m_sIPAddr;
  std::string          m_sPortNo;
  
protected:


public:

  cCmdLineParser();
  ~cCmdLineParser();
  
  void usage();
  
  int parse_verify_encode_args(int argc, char* argv[]);
  
  const char* getIPAddress_string();  
  const char* getPortNo_string();
  const char* getSocketType_string();
  const char* getNetworkDomain_string();
  
  int getSocketType_number();
  int getNetworkDomain_number();
  
  int setPortNo_string(std::string& sPortNo);
  int setIPAddress_string(std::string& sIPAddr);
  
};

#endif /* _C_CMDLINEPARSER_H */

