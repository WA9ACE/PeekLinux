




#ifndef __FSM_DEV_H__

#define __FSM_DEV_H__


#include "fsmdefs.h"


/**************************************************
 *
 * For all devices and all device drivers
 *
 **************************************************/

enum
{
	DEV_TYPE_FLASH = 0,
	DEV_TYPE_SD,
	DEV_TYPE_NAND_FLASH
};


/* Hardware Error Values */
enum
{
	HW_ERR_NONE = 0,
	HW_ERR_PARAM,
	HW_ERR_READ,
	HW_ERR_WRITE,
	HW_ERR_ERASE,
	HW_ERR_SYSTEM,
	HW_ERR_SUSPEND
};


/* Device control command. */
enum
{
	DEV_CTRL_DEVICE_DETACH	= 1,
	DEV_CTRL_END			= 2 /* The last common code, device-specific code will follow this code. */
};


struct _FSM_DEV_DRV;

typedef struct
{
	/* char				DevName[16];  	 DevName field is not used by now. */

	struct _FSM_DEV_DRV *	DevDrvP;
	uint8					DevType;
	void		  *			MediaObjP;		/* Reserved for File system use. */
} FsmDevObjHdrT;


typedef uint32 (*DEV_READ_FUNCPTR)(FsmDevObjHdrT * , uint8 * , uint32 , uint32 );

typedef uint32 (*DEV_WRITE_FUNCPTR)(FsmDevObjHdrT * , uint8 * , uint32 , uint32 );

typedef uint32 (*DEV_COPY_FUNCPTR)(FsmDevObjHdrT * , uint32 , uint32 , uint32 );

typedef uint32 (*DEV_ERASE_FUNCPTR)(FsmDevObjHdrT * , uint32 );

typedef uint32 (*DEV_CTRL_FUNCPTR)(FsmDevObjHdrT * , uint32 , void *);

typedef uint32 (*DEV_INIT_FUNCPTR)(FsmDevObjHdrT * );


typedef struct _FSM_DEV_DRV
{
	DEV_READ_FUNCPTR		FsmDevRead;
	DEV_WRITE_FUNCPTR		FsmDevWrite;
	DEV_COPY_FUNCPTR		FsmDevCopy;
	DEV_ERASE_FUNCPTR		FsmDevErase;
	DEV_CTRL_FUNCPTR		FsmDevCtrl;
	DEV_INIT_FUNCPTR		FsmDevInit;

	/*
	uint32 (*FsmDevRead)(void * DevObjP, uint8 * buf, uint32 Addr, uint32 length);
	uint32 (*FsmDevWrite)(void * DevObjP, uint8 * buf, uint32 Addr, uint32 length);
	uint32 (*FsmDevCopy)(void * DevObjP, uint32 DstAddr, uint32 SrcAddr, uint32 length);
	uint32 (*FsmDevErase)(void * DevObjP, uint32 Addr);
	uint32 (*FsmDevCtrl)(void * DevObjP, uint32 CtrlCode, void *arg);
	uint32 (*FsmDevInit)(void * DevObjP);
	*/

} FsmDevDrvT;


#define VALIDATE_ADDRESS


#endif /* __FSM_DEV_H__ */




