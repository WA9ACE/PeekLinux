#ifndef CPVER_H
#define CPVER_H

#include "sysdefs.h"

#ifdef CUST_H
#include "custbtc.h"
#endif 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
  uint32 FourBytesForBoundary;
  char   SwVersionInforTag[24];  
  char   CustomerName[20];           /* Nokia/Samsung etc*/
  char   ProductName[10];            /*different CP should be assigned different numbers,*/
                                     /*customer should be responsible for the management.*/
  char   BaseBandID[10];             /*baseband id include the following:*/
                                     /*CDMA  series:TC7740,TC7741,TC7750,TC7751,TC7760,TC7770*/
                                     /*GSM   series:TG3310,TG3360,TG3361,TG3362,TG3370,TG3380*/
                                     /*AP    series:TM2281,TM2282,TM2283,TM2284*/
                                     /*WCDMA series:TW9990*/
  uint16 MajorVersion;               /* Major CP version, customer should be in charge of the management.*/
  uint16 MinorVersion;               /*Minor CP version, customer should be in charge of the management.*/
  uint16 ReviseVersion;               /*Revise CP version, customer should be in charge of the management.*/
}SwVersionInfoT;

SwVersionInfoT GetCustomerVersion(void);
#if defined(DENALI_BUILD_VERSION)
char* GetSWVersion();
char* GetHWVersion();
#endif /*DENALI_BUILD_VERSION*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

