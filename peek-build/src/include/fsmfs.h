




#ifndef __FSM_FS_H__

#define __FSM_FS_H__

#include "fsmdefs.h"
#include "fsmdev.h"


typedef struct
{
   FsmDevObjHdrT *DevObjP;
} FsmFileDescriptorHdrT;



typedef struct
{
	FsmDevObjHdrT  *DevObjP;
} FsmMediaObjHdrT;



typedef  uint32   (*FSM_CREATE_FUNCPTR)(FsmDevObjHdrT * , char * , uint32 , uint32 * );

typedef  uint32   (*FSM_OPEN_FUNCPTR)(FsmDevObjHdrT * , char * , uint32 , uint32 * );

typedef  uint32   (*FSM_DELETE_FUNCPTR)(FsmDevObjHdrT * , char * );

typedef  uint32   (*FSM_CLOSE_FUNCPTR)(uint32 );

typedef  uint32   (*FSM_READ_FUNCPTR)(uint32 , uint8 * , uint32 , uint32 * );

typedef  uint32   (*FSM_WRITE_FUNCPTR)(uint32 , uint8 * , uint32 , uint32 * );

typedef  uint32   (*FSM_IOCTRL_FUNCPTR)(uint32 , uint32 , void * );

typedef  uint32   (*FSM_INIT_FUNCPTR)(FsmDevObjHdrT * , uint32 );

typedef  uint32   (*FSM_TERMINATE_FUNCPTR)(FsmDevObjHdrT * );

typedef struct
{
   FSM_CREATE_FUNCPTR   FsmCreate;
   FSM_OPEN_FUNCPTR     FsmOpen;
   FSM_DELETE_FUNCPTR   FsmDelete;
   FSM_CLOSE_FUNCPTR    FsmClose;
   FSM_READ_FUNCPTR     FsmRead;
   FSM_WRITE_FUNCPTR    FsmWrite;
   FSM_IOCTRL_FUNCPTR   FsmIoCtrl;
   FSM_INIT_FUNCPTR     FsmInit;
   FSM_TERMINATE_FUNCPTR   FsmTerminate;

	/*
	uint32	(*FsmCreate)(FsmDevObjHdrT * DevObjP, char * FileName, uint32 CreateFlag, uint32 * ErrorCodeP);
	uint32	(*FsmOpen)(FsmDevObjHdrT * DevObjP, char * FileName, uint32 OpenMode, uint32 * ErrorCodeP);
	uint32	(*FsmDelete)(FsmDevObjHdrT * DevObjP, char * FileName);
	uint32	(*FsmClose)(uint32 fd);
	uint32	(*FsmRead)(uint32 fd, uint8 * Buf, uint32 size, uint32 * ErrorCodeP);
	uint32	(*FsmWrite)(uint32 fd, uint8 * Buf, uint32 size, uint32 * ErrorCodeP);
	uint32	(*FsmIoCtrl)(uint32 fd, uint32 cmd, void * arg);
	uint32	(*FsmInit)(FsmDevObjHdrT * DevObjP, uint32 SectorSize);
	uint32	(*FsmTerminate)(FsmDevObjHdrT * DevObjP);
	*/
} FsmFsDrvT;


#endif /* __FSM_FS_H__ */




