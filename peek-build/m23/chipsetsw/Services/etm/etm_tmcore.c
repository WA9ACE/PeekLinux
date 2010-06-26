/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	tmcore.c 
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.2
 *

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  16/06/2003	Creation
 *  02/07/2003  Removed l1_config.TestMode check from CODEC Write
 *  24/10/2003  Added support for _misc_enable, _misc_param_rw, _tm_init,
 *              _tm_get and _tm_set. These functions are removed from TML1.
 *              ONLY for ETM packets.
 *  10/12/2003  Updated etm_version() regarding new structur of T_VERSION
 *  30/03/2004  Updated etm_dieID_read() func. regarding get die id for 16 bits
 *              instead of 8 bits.
 *  20/04/2004  The FIDs have been updated with new values.
 *  26/10/2006  Integrated support of 128 bit die ID
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/

#include "rv/rv_defined_swe.h"

#include "etm/etm.h"
#include "etm/etm_api.h"
#include "etm/etm_trace.h"
#include "etm/etm_tmcore.h"
#include "etm/etm_tmcore_misc.h"
#include "etm/etm_version.h"
#include "rv/rv_general.h"

//#include "/chipsetsw/drivers/drv_core/abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_I2c.h"

#include "ffs/ffs.h" // needed of getting of ffs version

#include "spi/spi_drv.h"
#include "l1tm_ver.h" // needed for l1tm versions
// #include "l1_ver.h" // Version of Layer 1
// #include "l1p_ver.h" // contain GPRS-VERSION
#include "armio/armio.h"// For GPIO


extern void tr_etm_init(unsigned int mask);
extern T_VERSION *l1s_get_version (void);

// Version of the ETM CORE module
// See the file etm_version.h


/******************************************************************************
 * DIE ID settings 
 *****************************************************************************/

/* DIE ID register */
#if ((CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 14)) //For D-Sample: $CHIPSET  =  8 (=10 for D-sample AMR).
  #define DIE_ID_REG         (MEM_DEV_ID0 | 0xF010)        //+ 0xFFFEF010 for Calypso
  #define DIE_ID_SIZE     4
#elif (CHIPSET == 12)                                      //For E-Sample: $CHIPSET   =  12.
  #define DIE_ID_REG         (0xFFFE0000 | 0xF004)         //+ 0xFFFEF004 for Calypso Plus
  #define DIE_ID_SIZE     4
#elif (CHIPSET == 15)  // For LoCosto
  #define DIE_ID_REG_LSB    (0xFFFE0000 | 0xF004)          // Locosto: die id bit  63 to 0
  #define DIE_ID_REG_MSB    (0xFFFE0000 | 0xF040)          // Locosto: die id bit 127 to 64
  #define DIE_ID_SIZE     4
#endif 


// rv_general.h !!!
#define DIE_ID_STREAM_TO_ARRAY(a, p, l) {register INT32 i; for (i = 0; i < l; i++) a[i] = *(UINT16*)(p)++;}


/******************************************************************************
 * Internal prototypes 
 *****************************************************************************/

//T_ETM_PKT *etm_core_setup(uint8 fid);
int etm_core(uint8 *buf, int size);

/******************************************************************************
 * Register the Core Module to the ETM database 
 *****************************************************************************/

int etm_core_init(void)
{
    int result; 

    result = etm_register("CORE", ETM_CORE, 0, 0, etm_core);
    return result;    
}


/******************************************************************************
 * Memory read/write Functions
 *****************************************************************************/

// Describe the payload of the mem protocol !!!!!!
// |--type(1)-|--partnum(1)-|--addr(4)-|--data(?)-|

int etm_mem(T_ETM_PKT *pkt, uint8 *buf)
{
   int num, unitsize, error;
   uint8 type, param;
   uint8  *mem8;
   uint16 *mem16;
   uint32 *mem32;
   uint32 addr, tmp;
   static unsigned char test_buf[64];

   param = unitsize = *buf & 0x3;
   if (unitsize == 0)
       unitsize = 4;

   type = *buf & 0x10;
   buf++;

   num = *buf++;
   addr = etm_get32(buf);
   buf += 4;

   tr_etm(TgTrCore, "ETM CORE: _mem: type(0x%x) addr(0x%x) partnum(%d)", type, addr, num);

   // Put 'parameter' in return packet
   if ((error = etm_pkt_put8(pkt, param)) < 0) {
       return error;
   }
      
   switch (type) {
   case 0: // READ(0x00)
       switch (unitsize) {
       case 1:
           mem8 = (uint8 *) addr;
           while (num--) {
               if ((error = etm_pkt_put8(pkt, *mem8++)) < 0)
                   break;
           }
           break;
       case 2:
           mem16 = (uint16 *) addr;
           while (num--) {
               if ((error = etm_pkt_put16(pkt, *mem16++)) < 0)
                   break;
           }
           break;
       case 4:
           mem32 = (uint32 *) addr;
           while (num--) {
               if ((error = etm_pkt_put32(pkt, *mem32++)) < 0)
                   break;
           }
           break;
       } 
       break;

   case 16: // WRITE(0x10)  
       switch (unitsize) {
       case 1:
           mem8 = (uint8 *) addr;
           while (num--) {
               *mem8++ = etm_get8(buf);
               buf += 1;
           }
           break;
       case 2:
           mem16 = (uint16 *) addr;
           while (num--) {
               *mem16++ = tmp = etm_get16(buf); 
               buf += 2;
           }
           break;
       case 4:
           mem32 = (uint32 *) addr;
           while (num--) {
               *mem32++ = etm_get32(buf); 
               buf += 4;
           }
           break;
       }
       break;
   default:
       return ETM_NOSYS;
   }
   
   if (error < 0)
       return error;
   
    return ETM_OK;
}


/******************************************************************************
 * CODEC Functions
 *****************************************************************************/
// Payload: | fid/8 | page/8 | reg/16 | data/16 |

int etm_codec_write(T_ETM_PKT *pkt, uint8 *buf)
{
    uint8 page, reg, regData;
    int result = 0;

    page    = etm_get8(buf);   buf += 1;
    reg     = etm_get16(buf);  buf += 2;
    regData = etm_get16(buf);
    
    if ((result = etm_pkt_put8(pkt, page)) < 0)
        return result;
    if ((result = etm_pkt_put16(pkt, reg)) < 0)
        return result;
   
    tr_etm(TgTrCore, "ETM CORE: _codec_write: page(%d) reg(%d) data(0x%x)", 
           page, reg, regData);

    if (page > 3 || reg > 255)
        return ETM_INVAL;

    result = BspTwl3029_I2c_WriteSingle(page, reg, regData, NULL);
    tr_etm(TgTrCore, "ETM CORE: _codec_write: returnCode(%d)", result);

    if (result == BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE)
        return ETM_AGAIN;
    if (result != BSP_I2C_RETURN_CODE_SUCCESS)
        return result;
    
    return ETM_OK;
}

int etm_codec_read(T_ETM_PKT *pkt, uint8 *buf)
{
//    volatile BspTwl3029_I2C_RegData regData = 0;
    volatile uint16 regData = 0; 
    uint16 page, reg;
    int result = 0;
    
    page = etm_get8(buf);  buf += 1;
    reg  = etm_get16(buf); 
    
    if ((result = etm_pkt_put8(pkt, page)) < 0)
        return result;
    if ((result = etm_pkt_put16(pkt, reg)) < 0)
        return result;
    
    tr_etm(TgTrCore, "ETM CORE: _codec_read: page(%d) reg(%d)", page, reg);

    if (page > 3 || reg > 255) {
        return ETM_INVAL;
    }

    result = BspTwl3029_I2c_ReadSingle(page, reg, (BspTwl3029_I2C_RegData*) &regData, NULL);
    tr_etm(TgTrCore, "ETM CORE: _codec_read: regData(0x%x), returnCode(%d)", regData, result);

    if (result == BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE)
        return ETM_AGAIN;
    if (result != BSP_I2C_RETURN_CODE_SUCCESS)
        return result;

    result = etm_pkt_put16(pkt, regData);
    return result;
}


/******************************************************************************
 *  Miscellaneous Functions - Imported from l1tm_func.c
 *****************************************************************************/

int etm_misc_enable(T_ETM_PKT *pkt, uint8 *inbuf)
{
    int16 index;
    
    index =  *inbuf++;
    etm_pkt_put8(pkt, index);  // Put index in return packet
    
    etm_tm_misc_enable(index);
    
  return ETM_OK;
}

int etm_misc_param_write(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern int etm_tm_misc_param_write(int16 index, uint16 value);
    int16 index;
    uint16 value;

    index = *inbuf++;
    value = etm_get16(inbuf);

    tr_etm(TgTrCore, "ETM CORE: _misc_param_write: index(0x%x) data(%d)", index, value);
    tr_etm_hexdump(TgTrEtmLow, inbuf, 2);

    etm_pkt_put8(pkt, index);  // Put index in return packet
    
    switch (index) {
    case ADC_ENA_FLAG:
        l1_config.adc_enable = value;
        break;
    default:
        etm_tm_misc_param_write(index, value);
    } // end switch
    
    return ETM_OK;
}

int etm_misc_param_read(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern int etm_tm_misc_param_read(int16 index);
    int error = ETM_OK;
    int16 index;
    volatile int value;

    index =  *inbuf++;
    etm_pkt_put8(pkt, index);  // Put index in return packet

    switch (index) {
    case ADC_ENA_FLAG:
        value = l1_config.adc_enable;
        break;
    case CURRENT_TM_MODE:  // index is read-only
        value = l1_config.TestMode;
        break;
    default:
        value = etm_tm_misc_param_read(index);
    } // end switch

    tr_etm(TgTrCore, "ETM CORE: _misc_param_read: index(0x%x) value(%d)", index, value);

    if (value < ETM_OK)
        return value;
    
    error = etm_pkt_put16(pkt, (uint16) value);

    return error;
}

int etm_tm_initialize(T_ETM_PKT *pkt, uint8 *inbuf)
{
    etm_tm_init();

  return ETM_OK;
}

int etm_tm_version_get(T_ETM_PKT *pkt, uint8 *inbuf)
{
    volatile int revision;
    T_VERSION *l1s_version;
    extern T_RF rf;
    int error = ETM_OK;
    uint8 index;

    index = *inbuf++;
    error = etm_pkt_put8(pkt, index);  // Put index in return packet
   
    l1s_version = (T_VERSION*) l1s_get_version(); 
    
    switch(index)
    {
        /*
          case BOARDHW_MODULE_REV:
          return E_NOSYS;
          break;
        */
    case BBCHIP_MODULE_REV:
        revision = CHIPSET;
        break;
#if (CODE_VERSION != SIMULATION)
    case CHIPID_MODULE_REV:
        revision = *( (volatile UWORD16 *) (MEM_JTAGID_PART));
        break;
    case CHIPVER_MODULE_REV:
        revision = *( (volatile UWORD16 *) (MEM_JTAGID_VER));
        break;
#endif
    case DSPSW_MODULE_REV:
        revision = l1s.version.dsp_code_version;
        break;
    case ANALOGCHIP_MODULE_REV:
        revision = ((ANLG_PG << 7) | ANLG_FAM);
        break;
    case GSM_MODULE_REV:
        revision = ALR;
        break;
    case LAYER1_MODULE_REV:
        revision = ((T_VERSION*) l1s_version)->mcu_tcs_official;
//        revision = l1s.version.mcu_alr_version; // old
        break;                
    case RFDRIVER_MODULE_REV:
        revision = rf.rf_revision;
      break;                
    case TM_API_MODULE_REV:
        revision = TMAPIVERSION;
        break;
    case L1_TM_CORE_MODULE_REV:
        revision = ((T_VERSION*) l1s_version)->mcu_tm_version;
//        revision = l1s.version.mcu_tm_version; // old
        break;
    case STD_MODULE_REV:
        revision = STD;
        break;
    case DSP_MODULE_REV:
        revision = DSP;
        break;
//    case BOARD_MODULE_REV:
//        revision = BOARD;
//        break;
    case RF_MODULE_REV: 
        revision = ((RF_PA << 10) | (RF_PG << 7) | RF_FAM);
        break;
    default:
        return ETM_INVAL;
    } // end switch

    error = etm_pkt_put16(pkt, (uint16) revision);

    return error;
}

int etm_tm_mode_set(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern void l1tm_initialize_var(void);
    volatile uint16 value;   
    int error = ETM_OK;
    uint8 index;
   
    index =  *inbuf++;
    etm_pkt_put8(pkt, index);  // Put index in return packet
 
    switch (index) {
    case 0: 
        l1_config.TestMode = 0;
        break;
    case 1: 
        l1_config.TestMode    = 1;
        l1_config.tx_pwr_code = 1;
        l1_config.pwr_mngt    = 0;
        
        etm_tm_init();
        l1tm_initialize_var(); // FixMe - port this to other function
        break;
    default:
        error = ETM_INVAL;
        break;
    }  // end switch
    
    return error;
}


/******************************************************************************
 * Echo and Reset Functions
 *****************************************************************************/

//structur of data dl: |delay|recvsize|num| = 3x2 bytes
int etm_echo(T_ETM_PKT *pkt, uint8 *data)
{
    int delay, sendsize, i, num, count;

    tr_etm(TgTrCore, "etm_echo:");

    delay = etm_get16(data);
    data += 2;

    sendsize = etm_get16(data);
    if (sendsize > 240)
        return ETM_INVAL;

    data += 2;
    num = etm_get16(data);

    tr_etm(TgTrCore, "ETM CORE: _echo: delay(%d) sendsize(%d) num(%d)", 
           delay, sendsize, num);

    if (delay > 0) {
        rvf_delay((delay + 32) * 14 / 64);
    }
    
    for (i = 0; i < sendsize; i++) {
        pkt->data[i+1] = i;        // data[0] = fid
    }

    pkt->size = sendsize; 

    return ETM_OK;
}

int etm_reset(void)
{
// The reset code is taken form Fluid->cmd.c
    int i;

    tr_etm(TgTrCore, "ETM CORE: _reset: Target is Reset");
    
    // Setup watchdog timer and let it timeout to make a reset
    *(volatile uint16*) 0xfffff804 = 0xFFFF;  // Timer to watchdog
    *(volatile uint16*) 0xfffff800 = 0x0080;  // Start timer
		// Apparently works it only if we read this register?
    i = *(volatile uint16*) 0xfffff802;
    *(volatile uint16*) 0xfffff802 = 0x0001;  // Load timer
    
    return ETM_OK;
}


/******************************************************************************
 * Set Test Controls 
 *****************************************************************************/

int etm_debug(T_ETM_PKT *pkt, uint8 *buf)
{
   int type, error, data;
   
   static char *p;

   type = *buf & 0x0F;   
   buf++; 

   data = etm_get32(buf);

   tr_etm(TgTrCore, "ETM CORE: _debug: type(%d) data(0x%x)", type, data);

   switch (type) {
   case 0: // (0x00) Allocate Test Buffer
       if ((p = etm_malloc(data)) == NULL)
           error = ETM_NOMEM;
       error = etm_pkt_put32(pkt, (int) p); 
       break;
   case 1: // (0x01) Free Test Buffer.
       p = (char *) data;
       etm_free(p);                         
       break;
   case 2: // (0x02) Set ETM Trace mask
       tr_etm_init(data);                   
       break;
   case 3: // (0x03) Set read all mem banks stat
       rvf_dump_mem();
       rvf_dump_pool();
       rvf_dump_tasks();
       break;
   default:
       error = ETM_NOSYS;                  
   }

   if (error < 0)
       return error;

    return ETM_OK;
}

/******************************************************************************
 * Get Version of ...
 *****************************************************************************/

int etm_version(T_ETM_PKT *pkt, uint8 *buf)
{
    extern uint16 etm_audio_revision;
    extern uint16 etm_task_revision;
    int error, fid, ffs_tm_version;
    volatile int revision = 0;
    T_VERSION *l1s_version;
//    uint16 data_struct_offset; // RemoveMe
//        data_struct_offset = (char*)&(l1s_p->version.mcu_tcs_program_release) - (char*)l1s_p;

    fid = etm_get32(buf);

    tr_etm(TgTrCore, "ETM CORE: _version: fid(0x%x)", fid);

    l1s_version = (T_VERSION*) l1s_get_version(); 

    switch (fid) {
// Code Versions related to ETM modules 
    case SW_REV_ETM_CORE:
        error = etm_pkt_put32(pkt, ETM_CORE_VERSION);
        break;
    case SW_REV_ETM_AUDIO:
        error = etm_pkt_put32(pkt, ETM_AUDIO_VERSION);
        break;
    case SW_REV_ETM_FFS:
        ffs_query(Q_FFS_TM_VERSION, &ffs_tm_version);
        error = etm_pkt_put32(pkt, ffs_tm_version);
        break;
    case SW_REV_ETM_RF: // Layer1 Testmode Version
        error = etm_pkt_put32(pkt, TESTMODEVERSION);
        break;
    case SW_REV_ETM_PWR:
        error = ETM_NOSYS;
        break;
    case SW_REV_ETM_BT:
        error = ETM_NOSYS;
        break;
    case SW_REV_ETM_TASK:
        error = etm_pkt_put32(pkt, ETM_VERSION);
        break;
    case SW_REV_ETM_API:
        error = etm_pkt_put32(pkt, ETM_API_VERSION); 
        break;
// Code Versions related to L1, see in l1_defty.h 
// Get the version on this way "revision = l1s.version.dsp_code_version;" 
// doesn't work because of struct aligment -> compile flag -mw !!!!
    case SW_DSP_CODE_VERSION:
        revision = ((T_VERSION*) l1s_version)->dsp_code_version;
        error = etm_pkt_put32(pkt, revision);
        break;
    case SW_DSP_PATCH_VERSION:
        revision = ((T_VERSION*) l1s_version)->dsp_patch_version;
        error = etm_pkt_put32(pkt, revision);
        break;
    case SW_MCU_TCS_PROGRAM_RELEASE:
        revision = ((T_VERSION*) l1s_version)->mcu_tcs_program_release;
        error = etm_pkt_put32(pkt, revision);
        break;
    case SW_MCU_TCS_OFFICIAL: // This version allso identify version of Layer1 
        revision = ((T_VERSION*) l1s_version)->mcu_tcs_official;
        error = etm_pkt_put32(pkt, revision);
        break;
    case SW_MCU_TCS_INTERNAL:
        revision = ((T_VERSION*) l1s_version)->mcu_tcs_internal;
        error = etm_pkt_put32(pkt, revision);
        break;
    case SW_MCU_TM_VERSION:
        revision = ((T_VERSION*) l1s_version)->mcu_tm_version;
        error = etm_pkt_put32(pkt, revision);
        break;
    default:
        error = ETM_NOSYS;
    }
    
    tr_etm(TgTrCore, "ETM CORE: _version: version(%d)", revision);

    if (error < 0)
        return error;
    
    return ETM_OK;
}


/******************************************************************************
 *  Function for reading the Die-ID from base band processor.
 *****************************************************************************/

int etm_dieID_read(T_ETM_PKT *pkt, uint8 *inbuf) 
{
	int result;
	int8 byteCount;
	uint16 dieID[DIE_ID_SIZE];
	int16 index;
    volatile UINT16 *reg_p;

#if ((CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 14) || (CHIPSET == 12))     
	reg_p = (UINT16 *) DIE_ID_REG;    

	tr_etm(TgTrCore, "ETM CORE: _dieID_read: bit 63 to 0 - Die-ID address(0x%x)", reg_p);

	DIE_ID_STREAM_TO_ARRAY(dieID, reg_p, DIE_ID_SIZE);
    
	for (byteCount = 0; byteCount < DIE_ID_SIZE; byteCount++) {
		tr_etm(TgTrCore, "ETM CORE: Die-ID[%i] Byte Read(0x%x)", byteCount, (UINT16)dieID[byteCount]);
        result = etm_pkt_put16(pkt, (UINT8)(((dieID[byteCount]) & 0xFFFF)));
		if (result < 0)
			return result;
	}

#elif (CHIPSET == 15)
	reg_p = (UINT16 *) DIE_ID_REG_LSB;
    
	tr_etm(TgTrCore, "ETM CORE: _dieID_read: CHIPSET 15 - bit 63 to 0");

	for (byteCount = 0; byteCount < DIE_ID_SIZE; byteCount++) {
		tr_etm(TgTrCore, "ETM CORE: Die-ID[0x%x = 0x%x]", reg_p, *reg_p);
        result = etm_pkt_put16(pkt, *reg_p);
        reg_p += 1;
		if (result < 0)
			return result;
	}

	reg_p = (UINT16 *) DIE_ID_REG_MSB;

	tr_etm(TgTrCore, "ETM CORE: _dieID_read: CHIPSET 15 - bit 127 to 64");

	for (byteCount = 0; byteCount < DIE_ID_SIZE; byteCount++) {
		tr_etm(TgTrCore, "ETM CORE: Die-ID[0x%x = 0x%x]", reg_p, *reg_p);
        result = etm_pkt_put16(pkt, *reg_p);
        reg_p += 1;
		if (result < 0)
			return result;
	}
#endif

	return ETM_OK;
}


/******************************************************************************
 * ETM CORE Main Function - Module
 *****************************************************************************/

int etm_core(uint8 *buf, int size)
{
// Structur of protocol data dl-link: |fid|index|data|

    uint8 mid;
    uint8 fid;
    int error = 0;
    T_ETM_PKT *pkt = NULL;  

    fid = *buf++;

    tr_etm(TgTrCore, "ETM CORE: _core: fid(%c):", fid); 

    /* Create TestMode return Packet */
    if ((pkt = (T_ETM_PKT *) etm_malloc(sizeof(T_ETM_PKT))) == NULL) {
        return ETM_NOMEM;
    }
        
    // Init. of return packet
    pkt->mid    = ETM_CORE;
    pkt->status = ETM_OK;
    pkt->size   = 0;
    pkt->index  = 0;
    etm_pkt_put8(pkt, fid);
   
    switch (fid) {
//    case 0x69: // old 'A'
//        break;
//    case 0x70: // old 'a'
//        break;
#ifdef RVM_ATP_SWE
case 0x60: // old 'G'
    error = etm_at(pkt, (char *) buf);
    break;
#endif

    case 0x61: // old 'M' 
        error = etm_mem(pkt, buf);
        break;
    case 0x62: // old 'E'
        error = etm_echo(pkt, buf);
        break;
    case 0x63: // old 'R'
        error = etm_reset();
        break;
    case 0x64: // old 'T' 
        error = etm_debug(pkt, buf);
        break;
    case 0x65: // old 'V' 
        error = etm_version(pkt, buf);
        break;
    case 0x66: // old 'C'
        error = etm_codec_read(pkt, buf);
        break;
    case 0x67: // old 'D' 
        error = etm_codec_write(pkt, buf);            
        break;
    case 0x68: // old 'd'
        error = etm_dieID_read(pkt, buf);
        break;
// OLD TM3 CIDs	
    case 0x20: // old 's'
        error = etm_tm_initialize(pkt, buf);
        break;
    case 0x21: // old 'S'
        error = etm_tm_mode_set(pkt, buf);
        break;
    case 0x22: // old 'v'
        error = etm_tm_version_get(pkt, buf);
        break;
    case 0x44: // old 'P'
        error = etm_misc_param_write(pkt, buf);
        break;
    case 0x45: // old 'p'
        error = etm_misc_param_read(pkt, buf);
        break;
    case 0x48: // old 'm'
        error = etm_misc_enable(pkt, buf);
        break;
    default:
        tr_etm(TgTrCore,"ETM CORE: _core: fid ERROR"); 
        error = ETM_NOSYS;
        break;
    }
    
    if (error < 0) {
        tr_etm(TgTrCore,"ETM CORE: _core: FAILED"); 
        pkt->status = -error;
    }
   
    // etm_at(): send func. is controlled by primitive 
    if (fid == 'G' && error >= RV_OK) {}
    else 
        etm_pkt_send(pkt);

    etm_free(pkt); 
    return ETM_OK;
}

