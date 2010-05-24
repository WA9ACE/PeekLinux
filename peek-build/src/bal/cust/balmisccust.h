



#ifndef  _BAL_MISC_CUST_H_
#define  _BAL_MISC_CUST_H_

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************

FUNCTION NAME:  BalMemoryPoolBufferGet

DESCRIPTION:    This routine is used to get the BAL MEMORY buffer point

PARAMETERS:     None

RETURNS:        pointer of the buffer where to create the val memory pool 

*****************************************************************************/
uint32 *BalMemoryPoolBufferGet(void);


/****************************************************************************

FUNCTION NAME:  BalMemoryPoolSizeGet

DESCRIPTION:    This routine is used to get size of the BAL MEMORY pool

PARAMETERS:     None

RETURNS:        size of the BAL MEMORY pool 

*****************************************************************************/
uint32 BalMemoryPoolSizeGet(void);

#ifdef __cplusplus
}
#endif
#endif


