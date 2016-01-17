#ifndef _COMMON_DEF_H
#define _COMMON_DEF_H

namespace commondef_namespace
{
 /* It tell what kind of connection client/server */
  enum eConnectionType
  {
    eCLIENT = 2,
    eSERVER = 4,
    eNONE  = 8
  };

 /* some the error code are defined*/
  enum eChatErrorCode
  {
    eSUCCESS                    =  0,
    eINPUT_PASSED_NOT_SUPPORTED = 2,
    eINPUT_VALIDATION_FAILED    = 4,
    eOBJECT_CREATION_FAILED     = 8,
    ePROTOCOL_NOT_SUPPORTED     = 16
  };
}



#endif /* _COMMON_DEF_H */

