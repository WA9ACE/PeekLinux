#ifndef _ASLLANGPACKAGE_H_
#define _ASLLANGPACKAGE_H_

#define CP2_HEADADDRESS 0x00550000
#define GETDATA_FUN_OFFSET 0xc
typedef enum 
{
    CP2_DATA_PRI,
    CP2_DATA_FONT,
    CP2_DATA_FONT12,
    CP2_DATA_FONT16,
    CP2_DATA_IMEDATA
}CP2_DATA_TYPE;

enum
{
    LANGPACKAGE_SUCCESS = 0,
    LANGPACKAGE_DATA_NOTFOUND,
    LANGPACKAGE_PACKAGE_NOTFOUND, 
    LANGPACKAGE_UNKNOWN_ERROR
};
 #ifdef __cplusplus
 extern "C"
 {
 #endif
 int GetCP2Data(CP2_DATA_TYPE nID, void **ppData);
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
#endif
