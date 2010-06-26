/************ Revision Controle System Header *************
 *                  GSM Layer 1 software
 *
 * DRP_DRIVE.C
 *
 *        Filename drp_drive.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define DRP_DRIVE_C

#include "sys_types.h"
#include "l1_confg.h"

#if (DRP_FW_BUILD==1)
  #include "l1_drp_if.h"
  #include "drp_fw_struct.h"
  #include "drp_l1_if.h"
  #include "drp_calib_main.h"
  #include "drp_main.h"
#endif
#include "drp_drive.h"

#include "drp_defines.h"

#include "l1_types.h"

#if (RF_FAM == 61)
  #include "l1_rf61.h"
#endif

#if (RF_FAM == 60)
  #include "l1_rf60.h"
#endif

#if (OP_L1_STANDALONE == 0) && (TESTMODE == 1)
  #include "ffs/ffs.h"
#endif

#include "drp_api.h"

#if (L1_DRP == 1)

#define MEM_READ_ERROR  -1
#define MEM_READ_OK 0

#define MEM_WRITE_ERROR  -1
#define MEM_WRITE_OK 0

#if (DRP_FW_BUILD==0)
SYS_UWORD32 drp_ref_sw_tag;
SYS_UWORD32 drp_ref_sw_ver;
#endif

#if (DRP_FW_BUILD==0)
extern T_DRP_SRM_API *drp_srm_api;
extern  T_DRP_REGS_STR *drp_regs;
extern T_DRP_SW_DATA drp_sw_data_calib;
extern T_DRP_SW_DATA drp_sw_data_calib_saved;
extern T_DRP_SW_DATA drp_sw_data_init;
#else
extern const T_DRP_SW_DATA drp_sw_data_init;
extern volatile const T_DRP_CERT_TABLE  drp_fw_certificate;
#endif
/*------------------------------------------------------------------*/
/* Function:void  drp_reg_on_script(UWORD8 SCRIPT_NUMBER) */

/*Functionality: Starts the REG ON DRP Script via writing the address of REG_ON Script to
                        SCR_START(0500)
                        To Enable the Script Processor: Write 1 in 7th bit
                        Bits 3210 should have the script #.
                        To enable any script Write 1 in 7th bit and Bits 3210 should have script # (0x1-0F)
    ........76543210
             1....0000
                        */
/*------------------------------------------------------------------*/
#if (DRP_FW_BUILD == 0)
void drp_script_run(SYS_UWORD32 script_num)
{
  volatile  SYS_UWORD32 *drp_scr_start;

  drp_scr_start = (SYS_UWORD32 *) (API_ADDRESS_DRP2ARM(DRP_SCR_START_ADDR));

//  drp_scr_start = (SYS_UWORD32 *) ( (DRP_SCR_START_ADDR) + ARM_DRP_OCP2_BASE_ADDRESS);

 // drp_scr_start = (SYS_UWORD32 *) (0xFFFF0500L);

  (*drp_scr_start) = (*drp_scr_start) & DRP_SCR_ENABLE_MASK;

  switch(script_num)
     {
        case DRP_REG_ON_SCRIPT:
   (*drp_scr_start) = (*drp_scr_start |DRP_REG_ON_SCRIPT | DRP_SCRIPT_START_MASK);
   break;

   case DRP_TX_ON_SCRIPT :
   (*drp_scr_start) = (*drp_scr_start |DRP_TX_ON_SCRIPT | DRP_SCRIPT_START_MASK);
   break;

   case DRP_RX_ON_SCRIPT :
        (*drp_scr_start) = (*drp_scr_start |DRP_RX_ON_SCRIPT |DRP_SCRIPT_START_MASK);
        break;

  case DRP_ROC_SCRIPT :
  (*drp_scr_start) = (*drp_scr_start |DRP_ROC_SCRIPT |DRP_SCRIPT_START_MASK);
   break;

       case DRP_IDLE_SCRIPT:
  (*drp_scr_start) = (*drp_scr_start |DRP_IDLE_SCRIPT |DRP_SCRIPT_START_MASK);
  break;

       case DRP_AFC_SCRIPT :
  (*drp_scr_start) = (*drp_scr_start |DRP_AFC_SCRIPT | DRP_SCRIPT_START_MASK);
  break;

       case DRP_REG_OFF_SCRIPT :
  (*drp_scr_start) = (*drp_scr_start |DRP_REG_OFF_SCRIPT |DRP_SCRIPT_START_MASK);
  break;

       case DRP_TEMP_MEAS_SCRIPT :
  (*drp_scr_start) = (*drp_scr_start |DRP_TEMP_MEAS_SCRIPT |DRP_SCRIPT_START_MASK);
  break;

     default:
   //ERROR Halt the Script Processor
   (*drp_scr_start) = (*drp_scr_start) & (DRP_SCR_DISABLE_MASK);
    }
}
#endif //#if (DRP_FW_BUILD == 0)


/*------------------------------------------------------------------*/
/* Function : DRP_Write_Uplink_Data    */


/*------------------------------------------------------------------*/
void  DRP_Write_Uplink_Data(SYS_UWORD16 *TM_ul_data)
{
  SYS_UWORD8  abb_table_index, abb_reg_bit_number, data_table_index, bit_number;
  SYS_UWORD16 data[10];
  volatile SYS_UWORD16 * drp_tx_reg_data_ptr ;

  abb_table_index = 0;
  abb_reg_bit_number = 15;

//  drp_tx_reg_data_ptr   =  (SYS_UWORD16 * )API_ADDRESS_DRP2ARM(DRP_TX_REG_DATA );
   drp_tx_reg_data_ptr   =  (SYS_UWORD16 * )(&(drp_regs->TX_DATAL));

  // abb_reg_bit_number 15   14   13   12   11   10   9   8   7   6   5   4   3   2   1    0   abb_table_index
  // TM_ul_data =        D    D    D    D    D    D   D   D   D   D   A   A   A   A   A    R/W      0
  //                     D    D    D    D    D    D   D   D   D   D   A   A   A   A   A    R/W      1
  //                                           .                                                    2
  //                                           .                                                    3
  //                                           .                                                    4
  //                                           .                                                    5
  //                                           .                                                    6
  //                                           .                                                    7
  //                                           .                                                    8
  //                                           .                                                    9
  //                                           .                                                    10
  //                                           .                                                    11
  //                                           .                                                    12
  //                                           .                                                    13
  //                                           .                                                    15
  //
  //                                                                                              data_table_index
  // data =               D(1,10),...............D(1,15)D(0,6)D(0,7)........................D(0,15)  0
  //                      D(3,14)D(3,15)D(2,6)............D(2,15)D(1,6).....................D(1,9)   1
  //                                                .                                                2
  //                                                .                                                3
  //                                                .                                                4
  //                                                .                                                5
  //                                                .                                                6
  //                                                .                                                7
  //                                                .                                                8
  //                                                .                                       D(15,15) 9
  // caption:
  //         D     : data
  //         A     : address
  //         R/W   : read/write ABB bit
  //         D(0,0): data of TM_ul_data bit number abb_reg_bit_number and table index abb_table_index

  for(data_table_index=0;data_table_index<10;data_table_index++)
  {
    // Reset the data
      data[data_table_index] = 0;
  }

  data_table_index = 0;

  for(bit_number=0;bit_number<=159;bit_number++)
  {
    data[data_table_index] |= ( ((TM_ul_data[abb_table_index]>>(abb_reg_bit_number)) & 0x0001) )<<(bit_number%16);

    if ((bit_number%16) == 15)
      data_table_index++;

    abb_reg_bit_number--;
    if (abb_reg_bit_number == 5)
  {
      abb_reg_bit_number = 15;
    abb_table_index++;
  }
  }

  for(data_table_index=0;data_table_index<10;data_table_index++)
  {
      // send the data to Tx buffer of DRP ... should write to LSB 16 of DRP_TX_REG_DATA
    (*drp_tx_reg_data_ptr)  = data[data_table_index];
  }
}

#if (DRP_FW_BUILD == 0)
/*------------------------------------------------------------------*/
/* drp_ref_sw_upload()                                              */
/*------------------------------------------------------------------*/
/* Parameters  :                                                    */
/*             1. sw :   DRP reference SW const array               */
/* -------------                                                    */
/* Return      :Success or ERROR                                    */
/*                  Returns 0 if Success else -1                    */
/* -------------                                                    */
/* Description :                                                    */
/* -------------                                                    */
/* This routine is used to load the reference SW from const array   */
/* to DRP SRM.The reference SW format is as following               */
/* sw format = {                                                    */
/* 4 bytes of Tags                                                  */
/* 4 bytes of Vers                                                  */
/* Block 1                                                          */
/* Block 2                                                          */
/* ......                                                           */
/* Block n                                                          */
/*                                                                  */
/* Block x                                                          */
/* 4 bytes of size                                                  */
/* 4 bytes of start address                                         */
/* payload                                                          */
/*                                                                  */
/* when x=n, size=0;                                                */
/* }                                                                */
/*------------------------------------------------------------------*/

SYS_WORD16 drp_ref_sw_upload(const SYS_UWORD8* sw )
{

  SYS_UWORD32 *drp_sw_ptr ;
  SYS_UWORD16 *dest_addr ,*src_addr;
  SYS_UWORD32 drp_addr ;

  SYS_UWORD8 ** sw_ptr ;
  SYS_UWORD16 blk_size, tm_tpcount;
 SYS_UWORD16 count;

/*
What we have here in the input array (U8 *) is the the Drp ref script which has 1st 32 bits as tag, 2nd 32 bits as Version , 3rd 32 bits as Size and the 4th 32 bit is a 32bit address (within the SRM) where the current function is supposed to write into.
It can only write the rest of the data in units of 16 bits
*/

//   *sw_ptr = (SYS_UWORD8 *) sw;
   drp_sw_ptr =  (SYS_UWORD32 *) sw;

  // read tag
  drp_ref_sw_tag = *drp_sw_ptr++;

  // read version
  drp_ref_sw_ver=*drp_sw_ptr++;

  // read size
  blk_size =*drp_sw_ptr++;

  while (blk_size != 0)
  {

  #if (DRP_MEM_SIMULATION)
     if (blk_size > DRP_SRM_SIZE)
      return MEM_WRITE_ERROR;
  #endif

  //read address
    drp_addr = *drp_sw_ptr++;

    // 16 bit OCP data
  #if (DRP_MEM_SIMULATION)
     dest_addr = (SYS_UWORD16 *) REF_SW_TO_DRP_SIMU_ADDR(drp_addr);
  #else
    dest_addr =(SYS_UWORD16 *) drp_addr;
  #endif

    src_addr   = (SYS_UWORD16 *) drp_sw_ptr;

    for (count=0; count < blk_size; count++)
    {
      #if (DRP_MEM_SIMULATION)

          *dest_addr++ = *src_addr++;

      #else // DRP_SIMULATION

        #if (RF_FAM==60)   // PLD board
          // transmit through USP interface
          PLD_WriteRegister(dest_addr++, *src_addr++);// Call USP driver function
        #elif (RF_FAM== 61)   // Locosto

          *dest_addr++ = *src_addr++;

        #endif

      #endif // DRP_MEM_SIMULATION

    }
    /// blk_size =*drp_sw_ptr++; probs here shd be src++

  drp_sw_ptr = ( (SYS_UWORD32 *) src_addr);
    blk_size =*drp_sw_ptr++;

  }
  return MEM_WRITE_OK;
}
#endif //#if (DRP_FW_BUILD == 0)

#if (OP_L1_STANDALONE == 0) && (TESTMODE == 1)
/*--------------------------------------------------------*/
/* drp_ref_sw_upload_from_ffs()                           */
/*--------------------------------------------------------*/
/* Parameters  :                                          */
/*             1. sw :   DRP reference SW const array     */
/* -------------                                          */
/* Return      :Success or ERROR                          */
/*                  Returns 0 if Success else -1          */
/* -------------                                          */
/* Description :                                          */
/* -------------                                          */
/* This routine is used to load the reference SW from FFS */
/* to DRP SRM.The reference SW format is as following     */
/* sw format = {                                          */
/* 4 bytes of Tags                                        */
/* 4 bytes of Vers                                        */
/* Block 1                                                */
/* Block 2                                                */
/* ......                                                 */
/* Block n                                                */
/*                                                        */
/* Block x                                                */
/* 4 bytes of size                                        */
/* 4 bytes of start address                               */
/* payload                                                */
/*                                                        */
/* when x=n, size=0;                                      */
/* }                                                      */
/*--------------------------------------------------------*/

SYS_WORD16 drp_ref_sw_upload_from_ffs(const SYS_UWORD8* pathname )
{


  SYS_UWORD16*  dest_addr;
  SYS_UWORD16  blk_size =0,count;
  SYS_WORD16 fd;
  SYS_WORD8 bytes=0 /*,error =0*/;
  SYS_UWORD32  drp_addr;
#if DRP_MEM_SIMULATION
  SYS_UWORD16  data; /* FOR COMPILATION */
#endif

  // Read the FFS file where reference SW is stored in FFS
  fd = ffs_open(((const char *)pathname),FFS_O_RDONLY);

  if (fd < 0)
  {
    // error
    return MEM_READ_ERROR;
  }

  // 16 bit OCP access and 32 bit SRM with 1 to 4 bytes Read/Write on OCP bus
  // Accodring to script format SRM data is 4 bytes.

  // read tag and write it to drp_ref_sw_tag
  bytes= ffs_read(fd,(void *)&drp_ref_sw_tag,4);
  if (bytes != 4)
  {
    return MEM_READ_ERROR;
  }

  // read Version and write it to the drp_ref_sw_tag
  bytes=ffs_read(fd,(void *)&drp_ref_sw_ver,4);
  if (bytes != 4)
  {
    return MEM_READ_ERROR;
  }

  // read Block size  and write it to the blk_size
  bytes= ffs_read(fd,(void *)&blk_size,4);
  if (bytes != 4)
  {
    return MEM_READ_ERROR;
  }

  // Copy DATA from FFS  to blk_dest_addr
  while (blk_size != 0)
  {
    //read address
    bytes=ffs_read(fd,(void *)&drp_addr,4);
    if (bytes != 4)
    {
      return MEM_READ_ERROR;
    }
    // 16 bit OCP data
  #if (DRP_MEM_SIMULATION)
     dest_addr = REF_SW_TO_DRP_SIMU_ADDR(drp_addr);
  #else
    #if (RF_FAM == 61)
      // DRP address needs to be changes to ARM address
//  API_ADDRESS_DRP2ARM(drp_addr); //OMAPS00090550
    #endif
    dest_addr =(SYS_UWORD16 *) drp_addr ;
  #endif

    for (count=0; count < blk_size; count++)
    {
      #if (DRP_MEM_SIMULATION)

        bytes=ffs_read(fd,(void *)dest_addr,2);
        if (bytes != 2)
        {
          return MEM_READ_ERROR;
        }
       dest_addr++;

      #else // DRP_MEM_SIMULATION

        #if (RF_FAM==60)   // PLD board
          // read from FFS first
          bytes=ffs_read(fd,(void *)&data,2);

      if (bytes <0 )
        {
          return MEM_READ_ERROR;
        }

          // transmit through USP interface
          PLD_WriteRegister(dest_addr, data);// Call USP driver function;
        #elif (RF_FAM== 61)   // Locosto
          bytes=ffs_read(fd,(void *)dest_addr,2);
          if (bytes != 2)
          {
            return MEM_READ_ERROR;
          }
         #endif

          dest_addr++;

      #endif // DRP_MEM_SIMULATION
    }
    //read next block size
    bytes= ffs_read(fd,(void *)&blk_size,4);
  }

  // Close the file
  ffs_close(fd);
  return MEM_READ_OK;//omaps00090550
}


#endif /* OP_L1_STANDALONE */

/*--------------------------------------------------------*/
/* drp_ref_sw_upload_from_ffs()                           */
/*--------------------------------------------------------*/
/* Parameters  :                                          */
/*             1. sw :   DRP reference SW const array     */
/* -------------                                          */
/* Return      :Success or ERROR                          */
/*                  Returns 0 if Success else -1          */
/* -------------                                          */
/* Description :                                          */
/* -------------                                          */
/* This routine is used to load the reference SW from FFS */
/* to DRP SRM.The reference SW format is as following     */
/* sw format = {                                          */
/* 4 bytes of Tags                                        */
/* 4 bytes of Vers                                        */
/* Block 1                                                */
/* Block 2                                                */
/* ......                                                 */
/* Block n                                                */
/*                                                        */
/* Block x                                                */
/* 4 bytes of size                                        */
/* 4 bytes of start address                               */
/* payload                                                */
/*                                                        */
/* when x=n, size=0;                                      */
/* }                                                      */
/*--------------------------------------------------------*/

SINT16 drp_sw_data_calib_upload_from_ffs(T_DRP_SW_DATA * ptr_drp_sw_data_calib)
{
#if (DRP_FW_BUILD == 1)
    //T_FFS_FD fd = 0;
    const char* pathname = "/sys/drp_calibration";
    SYS_UWORD32 bytes = 0;
    UINT16 drp_refsw_maj_version = ((*(drp_fw_certificate.drp_refsw_ver_mem)) >> 8) & 0xFF;
    UINT16 drp_calib_maj_version;
    UINT16 additional_struct_size = sizeof(T_DRP_SW_DATA_VER_EFUSE) + sizeof(T_DRP_PPA) + sizeof(T_FLYBACK_DELAY_CALIB);
    bytes = ffs_fread(pathname, (void *) ptr_drp_sw_data_calib, sizeof(T_DRP_SW_DATA));

    if (bytes == sizeof(T_DRP_SW_DATA))
    {
      if (ptr_drp_sw_data_calib->length != drp_sw_data_init.length)
      {
        memcpy(ptr_drp_sw_data_calib, &drp_sw_data_init, sizeof(T_DRP_SW_DATA));
        //drp_ver_efuse_read(ptr_drp_sw_data_calib);
      }
      //Calibration compatibility check
      drp_calib_maj_version = (((ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.version)) >> 8) & 0xFF;
      //if (drp_refsw_maj_version == drp_calib_maj_version)
        return MEM_READ_OK;
      //else
      //  return MEM_READ_ERROR;
    }
    //Old calibration data
    else if (bytes == (sizeof(T_DRP_SW_DATA) - additional_struct_size))
    {
      if (ptr_drp_sw_data_calib->length != (drp_sw_data_init.length - additional_struct_size))
      {
        memcpy(ptr_drp_sw_data_calib, &drp_sw_data_init, sizeof(T_DRP_SW_DATA));
      }
      return MEM_READ_OK;
    }
    else //Fresh calibration
    {
      memcpy(ptr_drp_sw_data_calib, &drp_sw_data_init, sizeof(T_DRP_SW_DATA));
      //drp_ver_efuse_read(ptr_drp_sw_data_calib);
      return MEM_READ_OK;
    }
#else // DRP_FW_BUILD==0

#if (OP_L1_STANDALONE == 1) && (TESTMODE == 1)
    DRP_UINT8  *ptrsrc, *ptrdst;
    DRP_UINT16 indx, strsize;
#endif

#if (OP_L1_STANDALONE == 0) && (TESTMODE == 1)
   const SYS_UWORD8* pathname = (SYS_UWORD8*)  "/sys/drp_calibration";
   SYS_UWORD32 bytes=0;
   SYS_WORD16  fd=0;

  // Read the FFS file where reference SW is stored in FFS
   fd = ffs_open(((const char *)pathname),FFS_O_RDONLY);

   if (fd < 0)
      return MEM_READ_ERROR;//OMAPS00090550

    // Copy DATA from FFS  to ptr_drp_sw_data_calib
    bytes = ffs_read(fd, (void *) ptr_drp_sw_data_calib, sizeof(drp_sw_data_calib));

    if (bytes != sizeof(drp_sw_data_calib))
     {
        return MEM_READ_ERROR;//OMAPS00090550
     }
     ffs_close(fd);
#endif // OP_L1_STANDALONE == 0) && (TESTMODE == 1)



#if (OP_L1_STANDALONE == 1) && (TESTMODE == 1)
  // Copy drp_sw_data_init into drp_sw_data_calib
  strsize = sizeof(T_DRP_SW_DATA);
  ptrsrc = (DRP_UINT8 *)(&drp_sw_data_init);
  ptrdst = (DRP_UINT8 *)(&drp_sw_data_calib);

    for(indx=0;indx < strsize;indx++)
    *ptrdst++ = *ptrsrc++;

      drp_copy_sw_data_to_drpsrm(&drp_sw_data_calib);
#endif
  return MEM_READ_OK;//omaps00090550
#endif // DRP_FW_BUILD
  }



#if (OP_L1_STANDALONE == 0) && (TESTMODE == 1)

SINT16 drp_sw_data_calib_upload_to_ffs(T_DRP_SW_DATA * ptr_drp_sw_data_calib)
{
   const SYS_UWORD8* pathname =  (SYS_UWORD8*) "/sys/drp_calibration";
   SYS_UWORD32 bytes=0;
   SYS_WORD16  fd=0;

  // Read the FFS file where reference SW is stored in FFS
   fd = ffs_open(((const char *)pathname),FFS_O_RDWR|FFS_O_CREATE);

   if (fd < 0)
      return MEM_READ_ERROR;//OMAPS00090550

    // Copy DATA from FFS  to ptr_drp_sw_data_calib
    bytes = ffs_write(fd, (void *) ptr_drp_sw_data_calib, sizeof(drp_sw_data_calib));

    if (bytes != sizeof(drp_sw_data_calib))
     {
      return MEM_READ_ERROR;//OMAPS00090550
     }
     ffs_close(fd);
     return MEM_READ_OK;//omaps00090550
  }

#endif /* OP_L1_STANDALONE */

#if (DRP_FW_BUILD == 1)
void drp_ver_efuse_read(T_DRP_SW_DATA * ptr_drp_sw_data_calib)
{
  //populate version info in drp_sw_data_init
  ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.version = *((UINT16 *)drp_fw_certificate.drp_refsw_ver_mem);
  ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.efuse[0] = drp_regs->EFUSE0L;
  ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.efuse[1] = drp_regs->EFUSE0H;
  ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.efuse[2] = drp_regs->EFUSE1L;
  ptr_drp_sw_data_calib->calib.drp_sw_data_ver_efuse.efuse[3] = drp_regs->EFUSE1H;

  return;
}
#endif //#if (DRP_FW_BUILD == 1)

#endif /*L1_DRP */

