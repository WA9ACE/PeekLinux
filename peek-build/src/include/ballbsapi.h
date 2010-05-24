#ifndef BALLBSAPI_H
#define BALLBSAPI_H


#include "sysdefs.h"
#include "balapi.h"

#define BAL_LBS_MAX_REG_TASK  2


/* values match return values in Brew AEEError.h API */
#define    LBS_SUCCESS            0     /* Success                      */
#define    LBS_FAIL               1     /* General failure              */
#define    LBS_CLASSNOTSUPPORT    3     /* Specified class unsupported  */

typedef enum 
{
    BAL_LBS_GET_GPS_INFO_MSG,
    BAL_LBS_GET_SECTOR_INFO_MSG,
    BAL_EVT_MAX
} BalLbsEventIdT;

typedef struct
{
    RegIdT RegId;
    uint16 Result;
    BalLbsSectorInfo Data;
} BalLbsGetSectorInfoT;

typedef struct
{
    uint32             dwTimeStamp;          /* Time, seconds since 1/6/1980                 */
    uint32             status;               /* Response status;                             */
    int32              dwLat;                /* Lat, 180/2^25 degrees, WGS-84 ellipsoid      */
    int32              dwLon;                /* Lon, 360/2^26 degrees, WGS-84 ellipsoid      */
    int16              wAltitude;            /* Alt, meters, WGS-84 ellipsoid                */
    uint16             wHeading;             /* Heading, 360/2^10 degrees                    */
    uint16             wVelocityHor;         /* Horizontal velocity, 0.25 meters/second      */
    int8               bVelocityVer;         /* Horizontal velocity, 0.25 meters/second      */
    uint8              accuracy;             /* Accuracy of the data                         */
    uint16             fValid;               /* Flags indicating valid fields in the struct. */
    uint8              bHorUnc;              /* Horizontal uncertainity                      */
    uint8              bHorUncAngle;         /* Hor. Uncertainity at angle                   */
    uint8              bHorUncPerp;          /* Hor. uncertainty perpendicular               */
    uint16             wVerUnc;              /* Vertical uncertainity.                       */
} BalLbsGPSInfo;

typedef struct
{
    RegIdT RegId;
    uint16 Result;
    BalLbsGPSInfo Data;
} BalLbsGetGpsInfoT;

typedef struct
{
    uint32 mode;      /* See AEEGPSMode (one shot, download first, track local ...)          */
    uint16 nFixes;
    uint16 nInterval;
    uint32 optim;     /* See AEEGPSOpt (speed, accuracy, default)                            */
    uint32 qos;       /* See AEEGPSQos (0 -> 255, with 255 being highest quality of service. */
    uint32 svrType;   /* Default: 0 / Server IP: 1 (Address provided below)                  */
    uint32 addr;      /* IP Address                                                          */
    uint16 port;      /* port number                                                         */
} BalGPSConfig;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

FUNCTION      BalLbsInit()

DESCRIPTION   Initialize the LBS Bal Module

PARAMETERS    None

RETURN VALUE  None

===========================================================================*/
void BalLbsInit( void );

/***************************************************************************

FUNCTION      BalLbsRegister

DESCRIPTION   This function registers a callback for all LBS related events.

PARAMETERS    NetCallback - The callback function to be called for the notification of LBS 
              services events.
    
RETURN VALUE  The assigned registration ID of the subscriber.

*****************************************************************************/
RegIdT BalLbsRegister( BalEventFunc Callback );

/***************************************************************************

FUNCTION      BalLbsUnregister

DESCRIPTION   This function unregisters the given client so that it won't receive additional
              LBS services related events.

PARAMETERS    RegId - The assigned registration ID of the subscriber.
  
RETURN VALUE  None 

*****************************************************************************/
void BalLbsUnregister( RegIdT RegId );

/*===========================================================================

FUNCTION:     BalLbsGetGPSInfo()
              
DESCRIPTION:  Bal API to begin LBS session over TCP/IP.
              Same as IPOSDET_GetGPSInfo() in Brew AEE API.

PARAMETERS:   RegIdT RegId 

RETURN VALUE: None

===========================================================================*/
void BalLbsGetGPSInfo( RegIdT RegId, BalGPSConfig* Config );

#ifdef SYS_OPTION_HL
/*===========================================================================

FUNCTION:     BalLbsGetSectorInfo()
              
DESCRIPTION:  Same as IPOSDET_GetSectorInfo() in Brew AEE API.

PARAMETERS:   RegIdT RegId 

RETURN VALUE: None

===========================================================================*/
void BalLbsGetSectorInfo( RegIdT RegId );
#endif

/*===========================================================================

FUNCTION:     BalLbsGetSap()
              
DESCRIPTION:  Return BalLocSapId 

PARAMETERS:   None 

RETURN VALUE: UINT8 BalLocSapId 

===========================================================================*/
uint8 BalLbsGetSap( void );

/*===========================================================================

FUNCTION:     BalLbsProcessEvent
              
DESCRIPTION:  This function processes the event received and calls the 
              registered call back.
  
PARAMETERS:   BalLbsEventIdT EventId, void *EventMsgP

RETURN VALUE: None

===========================================================================*/
void BalLbsProcessEvent( BalLbsEventIdT EventId, void* EventMsgP );

#ifdef  __cplusplus
}
#endif

#endif /*__BAL_LBS_H__*/





