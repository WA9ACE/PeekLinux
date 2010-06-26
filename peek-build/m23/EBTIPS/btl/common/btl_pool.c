/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_pool.c
*
*   DESCRIPTION:
*
*	Represents a pool of elements that may be allocated and released.
*
*	A pool is created with the specified number of elements. The memory for the elements
*	is specified by the caller as well.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include <bttypes.h>
#include <btl_pool.h>
#include <btl_defs.h>

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

static const U32 BTL_POOL_INVALID_ELEMENT_INDEX = BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS + 1;
static const U8 BTL_POOL_FREE_ELEMENT_MAP_INDICATION = 0;
static const U8 BTL_POOL_ALLOCATED_ELEMENT_MAP_INDICATION = 1;
static const U8 BTL_POOL_FREE_MEMORY_VALUE = 0x55;

static BOOL BTL_POOL_IsDestroyed(const BtlPool *pool);
static U32 BTL_POOL_GetFreeElementIndex(BtlPool *pool);
static U32 BTL_POOL_GetElementIndexFromAddress(BtlPool *pool, void *element);
static void* BTL_POOL_GetElementAddressFromIndex(BtlPool *pool, U32 elementIndex);

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

static void BtlPoolDebugAddPool(BtlPool *pool);

BtStatus BTL_POOL_Create(	BtlPool		*pool, 
								const char 	*name, 
								U32 			*elementsMemory, 
								U32 			numOfElements, 
								U32 			elementSize)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	U32 allocatedSize;
	
	BTL_FUNC_START("BTL_POOL_Create");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != name), BT_STATUS_INTERNAL_ERROR, ("Null name argument"));
	BTL_VERIFY_FATAL((0 != elementsMemory), BT_STATUS_INTERNAL_ERROR, ("Null elementsMemory argument"));
	BTL_VERIFY_FATAL((BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS > numOfElements), BT_STATUS_INTERNAL_ERROR, 
				("Max num of pool elements (%d) exceeded (%d)", numOfElements, BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS));
	BTL_VERIFY_FATAL((0 < elementSize), BT_STATUS_INTERNAL_ERROR, ("Element size must be >0"));
	
	allocatedSize = BTL_POOL_ACTUAL_SIZE_TO_ALLOCATED_MEMORY_SIZE(elementSize);

	/* Copy init values to members */
	pool->elementsMemory = elementsMemory;
	pool->elementAllocatedSize = allocatedSize;
	pool->numOfElements = numOfElements;
	
	/* Safely copy the pool name */
	OS_StrnCpy(pool->name, name, BTL_POOL_MAX_POOL_NAME_LEN);
	pool->name[BTL_POOL_MAX_POOL_NAME_LEN] = '\0';

	/* Init the other auxiliary members */
	
	pool->numOfAllocatedElements = 0;

	/* Mark all entries as free */
	OS_MemSet(pool->allocationMap, BTL_POOL_FREE_ELEMENT_MAP_INDICATION,  BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS);

	/* Fill memory in a special value to facilitate identification of dangling pointer usage */
	OS_MemSet((U8 *)pool->elementsMemory, BTL_POOL_FREE_MEMORY_VALUE, numOfElements * allocatedSize);

	BtlPoolDebugAddPool(pool);
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_Destroy(BtlPool	 *pool)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	U32		numOfAllocatedElements;
	
	BTL_FUNC_START("BTL_POOL_Destroy");

	BTL_LOG_INFO(("Destroying Pool %s", pool->name));
		
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));

	/* It is illegal to destroy a pool while there are allocated elements in the pool */
	status = BTL_POOL_GetNumOfAllocatedElements(pool, &numOfAllocatedElements);
	BTL_VERIFY_ERR_NORET(	(0 == numOfAllocatedElements), 
							("Pool %s still has %d allocated elements", pool->name, numOfAllocatedElements));

	pool->elementsMemory = 0;
	pool->numOfElements = 0;
	pool->elementAllocatedSize = 0;
	pool->numOfAllocatedElements = 0;
	OS_StrCpy(pool->name, "");
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_Allocate(BtlPool *pool, void ** element)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	U32 		allocatedIndex = BTL_POOL_INVALID_ELEMENT_INDEX;

	BTL_FUNC_START("BTL_POOL_Allocate");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != element), BT_STATUS_INTERNAL_ERROR, ("Null element argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));
		
	allocatedIndex = BTL_POOL_GetFreeElementIndex(pool);
	
	if (BTL_POOL_INVALID_ELEMENT_INDEX == allocatedIndex)
	{
		return BT_STATUS_NO_RESOURCES;
	}

	++(pool->numOfAllocatedElements);
	pool->allocationMap[allocatedIndex] = BTL_POOL_ALLOCATED_ELEMENT_MAP_INDICATION;

	*element = BTL_POOL_GetElementAddressFromIndex(pool, allocatedIndex);
 
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_Free(BtlPool *pool, void **element)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	/* Map the element's address to a map index */ 
	U32 freedIndex = BTL_POOL_INVALID_ELEMENT_INDEX;

	BTL_FUNC_START("BTL_POOL_Free");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != element), BT_STATUS_INTERNAL_ERROR, ("Null element argument"));
	BTL_VERIFY_FATAL((0 != *element), BT_STATUS_INTERNAL_ERROR, ("Null *element argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));
	
	freedIndex = BTL_POOL_GetElementIndexFromAddress(pool, *element);

	BTL_VERIFY_FATAL((BTL_POOL_INVALID_ELEMENT_INDEX != freedIndex), BT_STATUS_INTERNAL_ERROR,
						("Invalid element address"));
	BTL_VERIFY_FATAL(	(BTL_POOL_ALLOCATED_ELEMENT_MAP_INDICATION == pool->allocationMap[freedIndex]),
						BT_STATUS_INTERNAL_ERROR, ("Invalid element address"));
	
	pool->allocationMap[freedIndex] = BTL_POOL_FREE_ELEMENT_MAP_INDICATION;
	--(pool->numOfAllocatedElements);

	/* Fill memory in a special value to facilitate identification of dangling pointer usage */
	OS_MemSet(	(U8 *)BTL_POOL_GetElementAddressFromIndex(pool, freedIndex), 
			BTL_POOL_FREE_MEMORY_VALUE, pool->elementAllocatedSize);

	/* Make sure caller's pointer stops pointing to the freed element */
	*element = 0;
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_GetNumOfAllocatedElements(const BtlPool *pool, U32 *num)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_POOL_GetNumOfAllocatedElements");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));
	
	*num = pool->numOfAllocatedElements;

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_GetCapacity(const BtlPool *pool, U32 *capacity)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_POOL_GetCapacity");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != capacity), BT_STATUS_INTERNAL_ERROR, ("Null capacity argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));
	
	*capacity = pool->numOfElements;

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_IsFull(BtlPool *pool, BOOL *answer)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_POOL_IsFull");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != answer), BT_STATUS_INTERNAL_ERROR, ("Null answer argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));

	if (pool->numOfAllocatedElements == pool->numOfElements)
	{
		*answer = TRUE;
	}
	else
	{
		*answer = FALSE;
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_POOL_IsElelementAllocated(BtlPool *pool, const void* element, BOOL *answer)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	/* Map the element's address to a map index */ 
	U32 freedIndex = BTL_POOL_INVALID_ELEMENT_INDEX;
	
	BTL_FUNC_START("BTL_POOL_IsElelementAllocated");
	
	BTL_VERIFY_FATAL((0 != pool), BT_STATUS_INTERNAL_ERROR, ("Null pool argument"));
	BTL_VERIFY_FATAL((0 != element), BT_STATUS_INTERNAL_ERROR, ("Null element argument"));
	BTL_VERIFY_FATAL((0 != answer), BT_STATUS_INTERNAL_ERROR, ("Null answer argument"));
	BTL_VERIFY_FATAL((FALSE == BTL_POOL_IsDestroyed(pool)), BT_STATUS_INTERNAL_ERROR, ("Pool Already Destroyed"));

	freedIndex = BTL_POOL_GetElementIndexFromAddress(pool, (void*)element);

	BTL_VERIFY_ERR((BTL_POOL_INVALID_ELEMENT_INDEX != freedIndex), BT_STATUS_INVALID_PARM,
					("Invalid element address"));

	if (BTL_POOL_ALLOCATED_ELEMENT_MAP_INDICATION == pool->allocationMap[freedIndex])
	{
		*answer = TRUE;
	}
	else
	{
		*answer = FALSE;
	}

	BTL_FUNC_END();
	
	return status;
}

U32 BTL_POOL_GetFreeElementIndex(BtlPool *pool)
{	
	U32 freeIndex = BTL_POOL_INVALID_ELEMENT_INDEX;
	
	for (freeIndex = 0; freeIndex < pool->numOfElements; ++freeIndex)
	{
		if (BTL_POOL_FREE_ELEMENT_MAP_INDICATION == pool->allocationMap[freeIndex])
		{
			return freeIndex;
		}
	}
		
	return BTL_POOL_INVALID_ELEMENT_INDEX;
}

U32 BTL_POOL_GetElementIndexFromAddress(BtlPool *pool, void *element)
{
	U32 		elementAddress = (U32)element;
	U32 		elementsMemoryAddress = (U32)(pool->elementsMemory);
	U32 		index = BTL_POOL_INVALID_ELEMENT_INDEX;

	BTL_FUNC_START("BTL_POOL_GetElementIndexFromAddress");
	
	/* Verify that the specified address is valid (in the address range and on an element's boundary) */
	
	BTL_VERIFY_FATAL_SET_RETVAR(	(elementAddress >= elementsMemoryAddress), 
									index = BTL_POOL_INVALID_ELEMENT_INDEX,
									("Invalid element Address"));
	BTL_VERIFY_FATAL_SET_RETVAR(	(((elementAddress - elementsMemoryAddress) % pool->elementAllocatedSize) == 0),
									index = BTL_POOL_INVALID_ELEMENT_INDEX, 
									("Invalid element Address"));

	/* Calculate the index */
	index = (elementAddress - elementsMemoryAddress) / pool->elementAllocatedSize;

	/* Verify that its within index bounds */
	BTL_VERIFY_FATAL_SET_RETVAR((index < pool->numOfElements), index = BTL_POOL_INVALID_ELEMENT_INDEX, (""));

	BTL_FUNC_END();
	
	return index;
}

void* BTL_POOL_GetElementAddressFromIndex(BtlPool *pool, U32 elementIndex)
{
	/* we devide by 4 sinse pool->elementsMemory is U32 array .*/
	return &(pool->elementsMemory[elementIndex * pool->elementAllocatedSize / 4]);
}

/* DEBUG */

static BtlPool* BtlPoolDebugPools[20];
static BOOL BtlPoolDebugIsFirstPool = TRUE;

void BtlPoolDebugInit(void)
{
	OS_MemSet((U8*)BtlPoolDebugPools, 0, sizeof(BtlPool*));
}

void BtlPoolDebugAddPool(BtlPool *pool)
{
	U32	index = 0;
	
	if (TRUE == BtlPoolDebugIsFirstPool)
	{
		BtlPoolDebugInit();
		BtlPoolDebugIsFirstPool = FALSE;
	}

	for (index = 0; index < 20; ++index)
	{
		if (0 == BtlPoolDebugPools[index])
		{
			BtlPoolDebugPools[index] = pool;
			return;
		}
	}
	
}

void BTL_POOL_DEBUG_ListPools(void)
{
	U32	index = 0;

	BTL_FUNC_START("BTL_POOL_DEBUG_ListPools");

	BTL_LOG_DEBUG(("Pool List:\n"));
	
	for (index = 0; index < 20; ++index)
	{
		if (0 != BtlPoolDebugPools[index])
		{
			BTL_LOG_DEBUG(("Pool %s\n", BtlPoolDebugPools[index]->name));
		}
	}

	BTL_LOG_DEBUG(("\n"));

	BTL_FUNC_END();
}

void BTL_POOL_DEBUG_Print(char  *poolName)
{
	U32	index = 0;
	
	for (index = 0; index < 20; ++index)
	{
		if (0 != BtlPoolDebugPools[index])
		{
			if (0 == OS_StrCmp(poolName, BtlPoolDebugPools[index]->name))
			{
				BTL_LOG_DEBUG(("%d elements allocated in Pool %s \n", 
						BtlPoolDebugPools[index]->numOfAllocatedElements, BtlPoolDebugPools[index]->name));
				return;
			}
		}
	}
	
	BTL_LOG_DEBUG(("%s was not found\n", poolName));
}

BOOL BTL_POOL_IsDestroyed(const BtlPool *pool)
{
	if (0 != pool->elementsMemory)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


