#ifndef __BAL_OTASP_H__
#define __BAL_OTASP_H__

#define BAL_OTASP_MAX_REG_TASK   5


typedef enum
{
  BAL_OTASP_INDICATOR_MSG
} BalOtaspEventIdT;


#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================

FUNCTION BalOtaspInit

DESCRIPTION
  Initialize the OTASP module.

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void BalOtaspInit( void );

/*===========================================================================

FUNCTION BalOtaspRegister

DESCRIPTION
  register application to the Otasp module

DEPENDENCIES
  None

RETURN VALUE
  register id

===========================================================================*/
RegIdT BalOtaspRegister( BalEventFunc CallBack );

/*===========================================================================

FUNCTION BalOtaspUnregister

DESCRIPTION
  deregister application to the Otasp module

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void BalOtaspUnregister( RegIdT RegId );

void BalProcessOtaspIndMsg( void* MsgDataP );

#ifdef  __cplusplus
}
#endif

#endif

