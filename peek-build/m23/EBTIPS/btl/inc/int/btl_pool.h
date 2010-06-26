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
*   FILE NAME:      btl_pool.h
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


#ifndef __BTL_POOL_H
#define __BTL_POOL_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <btl_commoni.h>

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
/* Allocated memory is allways dividable by 4.*/
#define BTL_POOL_ACTUAL_SIZE_TO_ALLOCATED_MEMORY_SIZE(_actual_size)		((_actual_size) + 4 - ((_actual_size) % 4))

#define BTL_POOL_DECLARE_POOL(_pool_name, _memory_name, _num_of_elements, _element_size)	\
					BtlPool _pool_name;		\
					U32 _memory_name[(_num_of_elements) * (BTL_POOL_ACTUAL_SIZE_TO_ALLOCATED_MEMORY_SIZE(_element_size)) / 4]

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * _BtlPoolConstants constants enumerator
 *
 *     Contains constant values definitions
 */
enum _BtlPoolConstants
{
	BTL_POOL_MAX_POOL_NAME_LEN = 20,
	BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS = 120
};

/*-------------------------------------------------------------------------------
 * BtlPool structure
 *
 *     Contains a pool "class" members
 */
typedef struct _BtlPool
{
	/* external memory region from which elements are allocated */
	U32		*elementsMemory;

	/* number of elements in the pool */ 
	U32		numOfElements;

	/* size of a single element in bytes */
	U32		elementAllocatedSize;

	/* number of currently allocated elements */ 
	U32		numOfAllocatedElements;

	/* pool name (for debugging */
	char		name[BTL_POOL_MAX_POOL_NAME_LEN + 1];

	/* Per-element allocation flag (TRUE = allocated) */
	U8		allocationMap[BTL_POOL_MAX_NUM_OF_POOL_ELEMENTS];
} BtlPool;

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_POOL_Create()
 *
 *		Create a new Pool instance.
 *
 * Parameters:
 *		pool [in / out] - Pool data ("this")
 *
 *		name [in] - pool name (for debugging)
 *
 *		elementsMemory [in] - external memory for allocation
 *
 *		numOfElements [in] - num of elements in the pool
 *
 *		elementSize [in] - size (in bytes) of a single element
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_Create(	BtlPool		*pool,
								const char 	*name,
								U32 			*elementsMemory,
								U32 			numOfElements,
								U32 			elementSize);

/*-------------------------------------------------------------------------------
 * BTL_POOL_Destroy()
 *
 *		Destroys an existing pool instance
 *
 * Parameters:
 *		pool [in / out] - Pool data ("this")
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_Destroy(BtlPool	 *pool);

/*-------------------------------------------------------------------------------
 * BTL_POOL_Allocate()
 *
 *		Allocate a free pool element.
 *
 * Parameters:
 *		pool [in / out] - Pool data ("this")
 *
 *		element [out] - allocated element
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_NO_RESOURCES - All elements are allocated
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_Allocate(BtlPool *pool, void ** element);

/*-------------------------------------------------------------------------------
 * BTL_POOL_Free()
 *
 *		Frees an allocated pool element.
 *
 * Parameters:
 *		pool [in / out] - Pool data ("this")
 *
 *		element [in / out] - freed element. Set to 0 upon exit to prevent caller from using it 
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_Free(BtlPool *pool, void **element);

/*-------------------------------------------------------------------------------
 * BTL_POOL_GetNumOfAllocatedElements()
 *
 *		Gets the number of allocated elements.
 *
 * Parameters:
 *		pool [in] - Pool data ("this")
 *
 *		num [out] - number of allocated elements 
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_GetNumOfAllocatedElements(const BtlPool *pool, U32 *num);

/*-------------------------------------------------------------------------------
 * BTL_POOL_GetCapacity()
 *
 *		Gets the capacity (max number of elements) of the pool.
 *
 * Parameters:
 *		pool [in] - Pool data ("this")
 *
 *		capacity [out] - max number of elements
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_GetCapacity(const BtlPool *pool, U32 *capacity);

/*-------------------------------------------------------------------------------
 * BTL_POOL_IsFull()
 *
 *		Checks if the pool is full (all elements are allocated)l.
 *
 * Parameters:
 *		pool [in] - Pool data ("this")
 *
 *		answer [out] - The answer:
 *						TRUE - pool is full (all elements are allocated)
 *						FALSE - pool is not full (there are unallocated elements)
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_IsFull(BtlPool *pool, BOOL *answer);

/*-------------------------------------------------------------------------------
 * BTL_POOL_IsElelementAllocated()
 *
 *		Checks if the specified element is allocated in the pool
 *
 * Parameters:
 *		pool [in] - Pool data ("this")
 *
 *		element [in] - Checked element
 *
 *		answer [out] - The answer:
 *						TRUE - element is allocated
 *						FALSE - element is NOT allocated
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_POOL_IsElelementAllocated(BtlPool *pool, const void* element, BOOL *answer);


/********************************************************************************
					DEBUG	UTILITIES
********************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_POOL_DEBUG_ListPools()
 *
 *		Debug utility that lists the names of all existing pools.
 *
 */
void BTL_POOL_DEBUG_ListPools(void);

/* Debug Utility - Print allocation state for a specific pool (identified by name) */
/*-------------------------------------------------------------------------------
 * BTL_POOL_DEBUG_Print()
 *
 *		Debug utility that prints information regarding the specified pool (by name)
 *
 * Parameters:
 *		poolName [in] - Pool name (as given during creation)
 *		
 */
void BTL_POOL_DEBUG_Print(char  *poolName);

#endif /* __BTL_POOL_H */



