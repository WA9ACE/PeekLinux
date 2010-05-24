/**
 * @file  mc_api.h
 *
 * API Definition for MC SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *  9/17/2003  ()   API function headers added.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_API_H_
#define __MC_API_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/*@}*/

/** event typedef */
typedef UINT16 T_MC_EVENTS;

/** Possible events*/
#define MC_EVENT_INSERTION   (0x01)
#define MC_EVENT_REMOVAL    (0x02)

/**
 * @name USB Event Type.
 *
 * This type is used to report the USB callback events.
 */
/*@{*/
typedef enum
{
	MC_INSERT_EVENT = 0,	//MC Inserted
	MC_REMOVE_EVENT		   //MC Removed

}T_MC_EVENT;
/*@}*/


/** used host controller frequency */
#define MC_CLK_SPEED     (1300000)


/** Memory Card type */
typedef enum {
    NO_CARD,
    SD_CARD,
    MMC_CARD
} T_MC_CARD_TYPE;

/** Mechanical write protection type of a SD card*/
typedef enum {
    NO_SD_CARD,
    PROTECTED,
    NOT_PROTECTED
} T_MC_SD_MECH_WP;

typedef UINT16 T_MC_RCA;

typedef UINT16 T_MC_SUBSCRIBER;

typedef enum {
    MC_RW_STREAM,
    MC_RW_BLOCK
} T_MC_RW_MODE;

typedef enum {
    CSD_ACTION_WRITE,
    CSD_ACTION_ERASE
} T_MC_CSD_ACTION;

/* Indicates whether to use DMA or let the CPU handle the copying. Possible
 * values
 */
typedef enum {
    MC_FORCE_CPU,  /* Use CPU to transfer MC-data to RAM */
    MC_FORCE_DMA,  /* Use DMA to transfer MC-data to RAM */
    MC_DMA_AUTO    /* Driver determines CPU or DMA transfer */
} T_MC_DMA_MODE;

typedef enum {
    CSD_FIELD_FILE_FORMAT_GRP,
    CSD_FIELD_COPY,
    CSD_FIELD_PERM_WRITE_PROTECT,
    CSD_FIELD_TMP_WRITE_PROTECT,
    CSD_FIELD_FILE_FORMAT,
    CSD_FIELD_FILE_ECC,
    CSD_FIELD_FILE_CRC
} T_MC_CSD_FIELD;


/**
 * @name USB event Callback type
 *
 * This is the type of callback to be registered for getting special USb event.
 *
 */
/*@{*/
typedef void (*T_MC_EVENT_CALLBACK)(T_MC_EVENT mcEvent, void *ctx);
/*@}*/



/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/**
 * Subscribe
 *
 * Detailled description.
 * This functions shall be used by a client to subscribe to the MC-driver.
 * After registration the client is able to use the driver services like
 * reading and writing data.
 *
 * @param   subscriber_p  Subscriber identification value, which shall be
 *                        allocated by the client and is filled by the driver.
 *          return_path   The return path of the client.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory. Client could not be subscribed
 *                        because maximum number of clients have been reached.
 */
extern T_RV_RET mc_subscribe (T_MC_SUBSCRIBER *subscriber_p,
                               T_RV_RETURN return_path);

/**
 * Unsubscribe
 *
 * Detailled description.
 * This function shall be used by a client to unsubscribe from the driver.
 * After calling this function the client will no longer able to use the
 * driver services.
 *
 * @param   subscriber_p  Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER The subscriber id is invalid.
 */
extern T_RV_RET mc_unsubscribe (T_MC_SUBSCRIBER *subscriber_p);


/**
 * API (bridge) Functions
 */

/**
 * @name usb_register_notifcation
 *
 * This function must be called by a Application to subscribe for USB event notification.
 *
 *
 * @param	T_USB_EVENT_CALLBACK: callback	the callback function pointer. This function
 *										    will be called for notifying USB events.
 * @param	void:				  ctx		This is the context information provided with callback.
 *											This will be provided transparently back to the caller.
 */
/*@{*/
T_RV_RET mc_register_notifcation(T_MC_EVENT_CALLBACK callback, void *ctx);
/*@}*/

/**
 * Read
 *
 * Detailled description.
 * This function reads data from a MC-card using a specific transfer mode.
 * If partial reads are allowed (if CSD parameter READ_BL_PARTIAL is set) the
 * start address can start and stop at any address within the card address
 * space, otherwise it shall start and stop at block boundaries. The client
 * is responsible for setting the correct mode, address and data size
 * parameter according to the device properties. The client can obtain these
 * properties by reading the CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          mode          The data transfer mode to use. The MC-specification
 *                        defines stream oriented data transfer and block
 *                        oriented data transfer. Only stream mode is supported
 *                        at this moment.
 *          addr          The physical start address in bytes units from where
 *                        to read data.
 *          data_p        Pointer to a destination buffer, provided by the
 *                        client, where the driver will put the data. The
 *                        buffer size shall be at least data_size bytes.
 *          data_size     Number of bytes to be read from the card.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_read (T_MC_RCA rca, T_MC_RW_MODE mode, UINT32 addr,
                          UINT8 *data_p, UINT32 data_size,
                          T_MC_SUBSCRIBER subscriber);

/**
 * Write
 *
 * Detailled description.
 * This function writes data to a MC-card using a specific transfer mode. If
 * partial reads are allowed (if CSD parameter READ_BL_PARTIAL is set) the
 * start address can start and stop at any address within the card address
 * space, otherwise it shall start and stop at block boundaries. The client
 * is responsible for setting the correct mode, address and data size parameter
 * according to the device properties. The client can obtain these properties
 * by reading the CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          mode          The data transfer mode to use. The MC-specification
 *                        defines stream oriented data transfer and block
 *                        oriented data transfer. Only stream mode is supported
 *                        at this moment.
 *          addr          The physical start address in bytes units from where
 *                        to write data.
 *          data_p        Pointer to a source buffer, provided by the client,
 *                        from where the driver will read the data. The buffer
 *                        size shall be at least data_size bytes.
 *          data_size     Number of bytes to be written to the card.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_write (T_MC_RCA rca, T_MC_RW_MODE mode, UINT32 addr,
                           UINT8 *data_p, UINT32 data_size,
                           T_MC_SUBSCRIBER subscriber);

/**
 * Erase
 *
 * Detailled description.
 * This function erases a range of erase groups on the card. The size of the
 * erase group is specified in the CSD. The erase group start and end address
 * is given in bytes units. This address will be rounded to down to the erase
 * group boundary.
 *
 * @param   rca           Relative Card Address.
 *          start_group   Erase group address in bytes units where erasing will
 *                        start.
 *          end_group     Erase group address in bytes units where erasing will
 *                        end.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_erase (T_MC_RCA rca, UINT32 erase_group_start,
                           UINT32 erase_group_end,
                           T_MC_SUBSCRIBER subscriber);

/**
 * Set write protect
 *
 * Detailled description.
 * This function sets the write protection of the addressed write protect
 * group against erase or write. The group size is defined in units of
 * WP_GRP_SIZE erase group as specified in the CSD. This function does not
 * write protect the entire card which can be done by setting the permanent or
 * temporary write protect bits in the CSD. For this the mc_write_CSD()
 * function shall be used.
 *
 * @param   rca           Relative Card Address.
 *          wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_set_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                       T_MC_SUBSCRIBER subscriber);

/**
 * Clear write protect
 *
 * Detailled description.
 * This function clears the write protection of the addressed write protect
 * group. The group size is defined in units of WP_GRP_SIZE erase group as
 * specified in the CSD. This function does not disable write protect of the
 * entire card which can be done by erasing the temporary write protect bits
 * in the CSD. For this the mc_write_CSD() function shall be used.
 *
 * @param   rca           Relative Card Address.
 *          wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_clr_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                       T_MC_SUBSCRIBER subscriber);

/**
 * return the mechanical write protection
 *
 * This function will return the mechanical write protection of
 * a SD-card.
 *
 * @param   rca         RCA of card
 *
 * @return  NO_SD_CARD
 *          PROTECTED
 *          NOT_PROTECTED
 */

/*@{*/
T_MC_SD_MECH_WP mc_sd_get_mech_wp(T_MC_RCA rca);

/**
 * Get write protect
 *
 * Detailled description.
 * This function reads 32 write protection bits representing 32 write protect
 * groups starting at the specified address.
 *
 * @param   rca           Relative Card Address.
 *          wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_get_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                       T_MC_SUBSCRIBER subscriber);

/**
 * Get card status
 *
 * Detailled description.
 * This function returns the 32-bit status register of the MC-card. This
 * status is not buffered in the driver but will be read directly from
 * the card. See [MC], paragraph 4.10 for an explanation of the status bits.
 *
 * @param   rca           Relative Card Address.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_get_card_status(T_MC_RCA rca,
                                    T_MC_SUBSCRIBER subscriber);

/**
 * Get controller status
 *
 * Detailled description.
 * This function returns 16-bit MC- host controller status register
 * [MC_STAT].
 *
 * @param   status        Pointer to status register allocated by client and
 *                        filled by driver. See [Host Controller],
 *                        paragraph 2.4 for an explanations of the status bits.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_get_controller_status (UINT16 *status);

/**
 * Enable DMA mode
 *
 * Detailled description.
 * This function selects the DMA mode to be used by the driver. Default setting
 * will be force CPU.
 *
 * @param   dma_mode      Indicates whether to use DMA or let the CPU handle
 *                        the copying.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 *          RV_INVALID_PARAMETER invalid mode.
 */
extern T_RV_RET mc_dma_mode (T_MC_DMA_MODE dma_mode);

/**
 * Update acquisition
 *
 * Detailled description.
 * This function starts an identification cycle of a card stack (acquisition
 * procedure). The card management information in the controller will be
 * updated. New cards will be initialised; old cards keep their configuration.
 * At the end all active cards are in Stand-by state.
 * After this function has completed the number of cards connected can be
 * retrieved with the mc_get_stack_size() function. The session address of
 * each connected card can be retrieved with the mc_read_card_stack() function.
 *
 * @param   subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 */
extern T_RV_RET mc_update_acq (T_MC_SUBSCRIBER subscriber);

/**
 * Reset
 *
 * Detailled description.
 * This function resets all cards to idle state. This function executes the
 * GO_IDLE_STATE command (CMD0) on the bus. After completion of this service
 * the mc_update_acq() function shall be called before the MC-cards can be
 * used.
 *
 * @param   subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 */
extern T_RV_RET mc_reset (T_MC_SUBSCRIBER subscriber);

/**
 * Get stack size
 *
 * Detailled description.
 * This function returns the number of connected MC-cards.
 *
 * @param   size_p        Pointer to integer value allocated by the client, in
 *                        which the driver stores the stack size.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER size_p is NULL.
 */
extern T_RV_RET mc_get_stack_size (UINT16 *size_p);

/**
 * Read card stack
 *
 * Detailled description.
 * This function returns the relative card address of each individual MC-card
 * on the MC-bus. The client needs to provide an array of T_MC_RCA. The size
 * of the array can be determined with the mc_get_stack_size() function.
 *
 * @param   stack_p       Pointer to T_MC_RCA array.
 *          size          Array size in units of T_MC_RCA.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER stack_p is NULL.
 */
extern T_RV_RET mc_read_card_stack (T_MC_RCA *stack_p, UINT16 size);

/**
 * Read OCR
 *
 * Detailled description.
 * This function returns the 32-bit OCR-register from a MC-card. This register
 * is not buffered in the driver and therefore will be read directly from the
 * card.
 *
 * @param   rca           Relative Card Address.
 *          ocr_p         Pointer to an 32-bits data location, provided by the
 *                        client, to which the driver copies the OCR.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_read_OCR (T_MC_RCA rca, UINT32 *ocr_p,
                              T_MC_SUBSCRIBER subscriber);

/**
 * Read CID
 *
 * Detailled description.
 * This function returns the 128-bit CID register from a MC-card. This
 * register is not buffered in the driver and therefore will be read
 * directly from the card.
 *
 * @param   rca           Relative Card Address.
 *          cid_p         Pointer to a 128-bit buffer, allocated by the
 *                        client, to which the driver copies the CID. cid_p
 *                        points to the MSB of the CID.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_read_CID (T_MC_RCA rca, UINT8 *cid_p,
                              T_MC_SUBSCRIBER subscriber);

/**
 * Read CSD
 *
 * Detailled description.
 * This function returns the 128-bit CSD register from a MC-card. This
 * register is not buffered in the driver and therefore will be read
 * directly from the card.
 *
 * @param   rca           Relative Card Address.
 *          csd_p         Pointer to a 128-bit buffer, allocated by the client,
 *                        to which the driver copies the CSD. csd_p points to
 *                        the MSB of the CSD.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_read_CSD (T_MC_RCA rca, UINT8 *csd_p,
                              T_MC_SUBSCRIBER subscriber);

/**
 * Write CSD
 *
 * Detailled description.
 * This function can be used to write one the programmable fields of the
 * CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          field         CSD field to write.
 *          value         Value to write.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_write_CSD (T_MC_RCA rca, T_MC_CSD_FIELD field,
                               UINT8 value, T_MC_SUBSCRIBER subscriber);

/**
 * Erase CSD
 *
 * Detailled description.
 * This function can be used to erase one the programmable fields of the
 * CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          field         CSD field to erase.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
extern T_RV_RET mc_erase_CSD (T_MC_RCA rca, T_MC_CSD_FIELD field,
                               T_MC_SUBSCRIBER subscriber);

/**
 * Get SW version
 *
 * Detailled description.
 * This function returns the software version of the driver.
 *
 * @return  UINT32  The 32-bit software version
 */
extern UINT32 mc_get_sw_version (void);

/**
 * Get HW version
 *
 * Detailled description.
 * This function returns the hardware version number (module revision )
 * of the controller. This number is stored in the Module Revision
 * Register (MC_REV) of the controller.
 *
 * @return  UINT16  The 16-bit hardware version
 */
extern UINT16 mc_get_hw_version (void);

/**
 * Subscribes the subscriber to MC events
 *
 * This function will subscribe the subscriber to certain events
 *
 *
 * @param   events      desired events:
 *                      MC_EVENT_INSERTION
 *                      MC_EVENT_REMOVAL
 * @param   subscriber  Subscriber ID
 *
 * @return  RV_OK
 *          RV_NOT_READY
 *          RV_INVALID_PARAMETER
 *          RV_MEMORY_ERR
 */
extern T_RV_RET  mc_send_notification(T_MC_EVENTS events, T_MC_SUBSCRIBER subscriber);


/**
 * Will retreive the 512 bit Status register of a SD card
 *
 * This function will retreive the 512 bit Status register of a SD card
 *
 * @param   relative card address number
 *
 * @return  RV_INVALID_PARAMETER  no card is available at given address (or invalid address)
            RV_INVALID_PARAMETER  there is a SD card attached
            MC_CARD there is a MC card attached
 */
extern T_RV_RET mc_sd_get_card_status(T_MC_RCA rca,UINT8 *sd_status_p,T_MC_SUBSCRIBER subscriber);
 /**
 * Returns the card type belonging to the given rca
 *
 * This function will return the card type belonging to the given
 * relative card address (rca).
 *
 *
 * @param   relative card address number
 *
 * @return  NO_CARD  no card is available at given address (or invalid address)
            SD_CARD  there is a SD card attached
            MC_CARD there is a MC card attached
 */
extern T_MC_CARD_TYPE mc_get_card_type(T_MC_RCA rca);

/**
 * Will retreive the 64 bit SCR register of a SD card
 *
 * This function will retreive the 64 bit SCR  register of a SD card
 *
 * @param   rca     relative card address number
 * @param   scr_p   reference to location where scr register can be stored
 * @param   subscriber id
 *
 * @return  RV_INVALID_PARAMETER  no card is available at given address (or invalid address)
            RV_INVALID_PARAMETER  there is no SD card attached
            MC_CARD there is a MC card attached
 */
extern T_RV_RET mc_read_scr(T_MC_RCA rca, UINT8 *scr_p,T_MC_SUBSCRIBER subscriber);

/*@}*/



/**defines needed by other components to use MC api*/
/*
#define T_MC_EVENTS                T_MC_EVENTS
#define MC_EVENT_INSERTION  MC_EVENT_INSERTION
#define MC_EVENT_REMOVAL  MC_EVENT_REMOVAL
#define MC_CLK_SPEED  MC_CLK_SPEED
#define T_MC_CARD_TYPE  T_MC_CARD_TYPE
#define T_MC_SD_MECH_WP  T_MC_SD_MECH_WP
#define T_MC_RCA  T_MC_RCA
#define T_MC_SUBSCRIBER  T_MC_SUBSCRIBER
#define MC_RW_STREAM MC_RW_STREAM
#define MC_RW_BLOCK MC_RW_BLOCK
#define T_MC_RW_MODE  T_MC_RW_MODE
#define T_MC_CSD_ACTION  T_MC_CSD_ACTION
#define MC_FORCE_CPU MC_FORCE_CPU
#define MC_FORCE_DMA MC_FORCE_DMA
#define MC_DMA_AUTO MC_DMA_AUTO
#define T_MC_DMA_MODE  T_MC_DMA_MODE
#define T_MC_CSD_FIELD  T_MC_CSD_FIELD
#define mc_subscribe  mc_subscribe
#define mc_unsubscribe   mc_unsubscribe
#define mc_read   mc_read
#define mc_write   mc_write
#define mc_erase   mc_erase
#define mc_set_write_protect   mc_set_write_protect
#define mc_clr_write_protect   mc_clr_write_protect
#define mc_sd_get_mech_wp  mc_sd_get_mech_wp
#define mc_get_write_protect   mc_get_write_protect
#define mc_get_card_status  mc_get_card_status
#define mc_get_controller_status   mc_get_controller_status
#define mc_dma_mode   mc_dma_mode
#define mc_update_acq   mc_update_acq
#define mc_reset   mc_reset
#define mc_get_stack_size   mc_get_stack_size
#define mc_read_card_stack   mc_read_card_stack
#define mc_read_OCR   mc_read_OCR
#define mc_read_CID   mc_read_CID
#define mc_read_CSD   mc_read_CSD
#define mc_write_CSD  mc_write_CSD
#define mc_erase_CSD  mc_erase_CSD
#define mc_get_sw_version   mc_get_sw_version
#define mc_get_hw_version   mc_get_hw_version
#define mc_send_notification  mc_send_notification
#define mc_sd_get_card_status  mc_sd_get_card_status
#define mc_get_card_type  mc_get_card_type
#define mc_read_scr  mc_read_scr
*/
/*@}*/
#ifdef __cplusplus
}
#endif


#endif /*__MC_API_H_*/

