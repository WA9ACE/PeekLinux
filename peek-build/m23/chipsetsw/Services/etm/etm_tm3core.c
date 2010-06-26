/********************************************************************************
 * Enhanced TestMode (ETM)
 * @file	_tm3core.c 
 *
 * @author	Kim T. Peteren (ktp@ti.com)
 * @version 0.2
 *
 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  02/12/2003	Creation
 *  03/12/2003  Finished integration of mixed TM3 functions
 *
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 *********************************************************************************/


#include "etm/etm.h"
#include "etm/etm_api.h"
#include "etm/etm_trace.h"
#include "etm/etm_tmcore.h"
#include "etm/etm_tmcore_misc.h"

#include "spi/spi_drv.h"
#include "l1_varex.h" // needed for l1 misc functions
#include "l1tm_ver.h" // needed for l1tm versions

//#include "armio/armio.h"    // needed for GPIO


extern void tr_etm_init(unsigned int mask);


/******************************************************************************
 * Internal prototypes 
 *****************************************************************************/

int etm_tm3core(uint8 *buf, int size);


/******************************************************************************
 * Register the Core Module to the ETM database 
 *****************************************************************************/

int etm_tm3core_init(void)
{
    int result; 

    result = etm_register("CORE", ETM_TM3_MISC, 0, 0, etm_tm3core);
    return result;    
}


/******************************************************************************
 * Memory read/write Functions
 *****************************************************************************/

// The payload of the tm3 mem_write protocol
// |--addr[4]-|--data(?)--| -> downlink payload
int etm_tm3_mem_write(T_ETM_PKT *pkt, uint8 *buf, int size)
{
   int num_of_bytes;
   uint8  *mem8;
   uint32 addr, tmp;

   num_of_bytes = size;
   addr = etm_get32(buf);
   buf += 4;

   num_of_bytes -= (4+2); // cid[1], addr[4], cksum[1] 

   tr_etm(TgTrCore, "ETM CORE: _tm3_mem_write: addr(0x%x) num_of_bytes(%d)", addr, num_of_bytes);

   mem8 = (uint8 *) addr;

   // We don't check if mem is RAM. That is up to the user.
   while (num_of_bytes--) {
       *mem8++ = etm_get8(buf);
       buf += 1;
   }
   
    return ETM_OK;
}

// The payload of the tm3 mem_read protocol
// |--addr[4]-|--data(?)-| -> downlink payload
int etm_tm3_mem_read(T_ETM_PKT *pkt, uint8 *buf)
{
   int num_of_bytes, error;
   uint8  *mem8 = NULL;
   uint32 addr;

   addr = etm_get32(buf);
   buf += 4;
   num_of_bytes = etm_get32(buf);

   tr_etm(TgTrCore, "ETM CORE: _tm3_mem_read: addr(0x%x) num_of_bytes(%d):", addr, num_of_bytes);

   if (num_of_bytes > 124) {
        return 7; //  7 = E_BADSIZE
   }

   etm_pkt_put32(pkt, num_of_bytes);
   mem8 = (uint8 *) addr;

   while (num_of_bytes--) {
       if ((error = etm_pkt_put8(pkt, *mem8++)) < 0)
           break;
   }
   
   return ETM_OK;
}


/******************************************************************************
 * CODEC Functions
 *****************************************************************************/

// ETM sends both page value and register address in one byte.
// Bit field is:  PPPR RRRR 0000 0000
// where P = page bit, R = register address bits and X = don't care bits.  

// The payload of the tm3 codec_write protocol
// |--reg+page[2]--|--data[2]--| -> downlink payload
int etm_tm3_codec_write(T_ETM_PKT *pkt, uint8 *buf)
{
    uint16  page, reg, data;
    int result, reg_data;

    reg_data = *buf++;

    page = (reg_data  >> 5) & 0x3;
    reg  =  reg_data        & 0x1F;

    buf++;
    data = etm_get16(buf);

    tr_etm(TgTrCore, "ETM CORE: _tm3_codec_write: page(%d) reg(%d) data(0x%x)",
           page, reg, (data & 0x3ff));

    if (page > 7 && reg > 32)
        return ETM_INVAL;
    else {
        // The function below expects a 1 for page 0 and a 2 for page 1.
        // The register address value is left-shifted by 1 since LSB is read/write command bit.
        // The value is written in the 10 MSB's of register.
#if (CHIPSET!=15)
        ABB_Write_Register_on_page(page + 1, reg << 1, (data & 0x3ff));
#endif
    }    

    return ETM_OK;
}

// The payload of the tm3 codec_read protocol
// |--reg+page[2]--| -> downlink payload
// |--data[2]--|     -> uplink payload
int etm_tm3_codec_read(T_ETM_PKT *pkt, uint8 *buf)
{
    volatile uint16 value;
    uint16 page, reg;
    int result, reg_data;
  
    reg_data = *buf;

    page = (reg_data  >> 5) & 0x3;
    reg  =  reg_data        & 0x1F;
    
    if (page > 7 && reg > 32)
        return ETM_INVAL;

    // The function below expects a 1 for page 0 and a 2 for page 1.
    // The register value is left-shifted by 1 since LSB is read/write command bit.
#if (CHIPSET!=15)
    value = ABB_Read_Register_on_page(page + 1, (reg << 1));
#endif
    
    tr_etm(TgTrCore, "ETM CORE: _codec_read: page(%d) reg(%d) value(0x%x)", page, reg, value);
    
    result = etm_pkt_put16(pkt, value);
    return result;
}


/******************************************************************************
 *  Miscellaneous Functions - Imported from l1tm_func.c
 *****************************************************************************/

// The payload of the tm3 misc_enable protocol
// |--index[2]--|  -> downlink payload
// |--index[1]--|  -> uplink payload
int etm_tm3_misc_enable(T_ETM_PKT *pkt, uint8 *inbuf)
{
    int16 index;
    
    index =  *inbuf++;
    etm_pkt_put8(pkt, index);  // Put index in return packet
    
    tr_etm(TgTrCore, "ETM CORE: _tm3_misc_enable: index(0x%x)", index);
    etm_tm_misc_enable(index);
    
  return ETM_OK;
}

// The payload of the tm3 misc_param_write protocol
// |--index[2]--|--data[2]--|  -> downlink payload
// |--index[1]--|              -> uplink payload
int etm_tm3_misc_param_write(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern int etm_tm_misc_param_write(int16 index, uint16 value);
    int16 index;
    uint16 value;

    index = etm_get16(inbuf);
    inbuf += 2;
    value = etm_get16(inbuf);

    tr_etm(TgTrCore, "ETM CORE: _tm3_misc_param_write: index(0x%x) data(%d)", index, value);
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

// The payload of the tm3 misc_param_read protocol
// |--index[2]--|              -> downlink payload
// |--index[1]--|--data[2]--|  -> uplink payload
int etm_tm3_misc_param_read(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern int etm_tm_misc_param_read(int16 index);
    int error = ETM_OK;
    int16 index;
    volatile int value;

    index = etm_get16(inbuf);
    inbuf += 2;
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

    tr_etm(TgTrCore, "ETM CORE: _tm3_misc_param_read: index(0x%x) value(%d)", index, value);

    if (value < ETM_OK)
        return value;
    
    error = etm_pkt_put16(pkt, (uint16) value);

    return error;
}

// The payload of the tm3 tm_initialize protocol
// |--(none)--|  -> downlink payload
// |--(none)--|  -> uplink payload
int etm_tm3_tm_initialize(T_ETM_PKT *pkt, uint8 *inbuf)
{
    etm_tm_init();
    return ETM_OK;
}

// The payload of the tm3 tm_version_get protocol
// |--index[2]--|            -> downlink payload
// |--index[1]--|--data[2]|  -> uplink payload
int etm_tm3_tm_version_get(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern T_RF rf;  
    extern T_VERSION *l1s_get_version (void);
    int error = ETM_OK;
    uint8 index;
    volatile int revision;
    T_VERSION *l1s_version = NULL;
  
    index = *inbuf++;
    error = etm_pkt_put8(pkt, index);  // Put index in return packet

    tr_etm(TgTrCore, "ETM CORE: _tm3_tm_version_get: index(%d)", index);
  
    switch(index)
    {
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
//        revision = l1s.version.mcu_tm_version;
        break;
    case STD_MODULE_REV:
        revision = STD;
        break;
    case DSP_MODULE_REV:
        revision = DSP;
        break;
    case RF_MODULE_REV: 
        revision = ((RF_PA << 10) | (RF_PG << 7) | RF_FAM);
        break;
    default:
        return ETM_INVAL;
    } // end switch

    error = etm_pkt_put16(pkt, (uint16) revision);

    return error;
}

// The payload of the tm3 tm_mode_set protocol
// |--index[2]--|  -> downlink payload
// |--index[1]--|  -> uplink payload
int etm_tm3_tm_mode_set(T_ETM_PKT *pkt, uint8 *inbuf)
{
    extern void l1tm_initialize_var(void);
    int error = ETM_OK;
    uint8 index;
    volatile uint16 value;

    index =  *inbuf++;
    etm_pkt_put8(pkt, index);  // Put index in return packet

    tr_etm(TgTrCore, "ETM CORE: _tm3_tm_mode_set: index(%d)", index);
 
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
 * ETM TM3 CORE Main Function - Module
 *****************************************************************************/

int etm_tm3core(uint8 *buf, int size)
{
// Structur of payload dl-link: |--cid--|--payload--|
// Structur of payload up-link: |--cid--|--status--|--payload--|

    uint8 mid;
    uint8 cid;
    int error = 0;
    T_ETM_PKT *pkt = NULL;  

    cid = *buf++;

    tr_etm(TgTrCore, "ETM CORE: _tm3core: cid(0x%x)", cid);

    /* Create TestMode return Packet */
    if ((pkt = (T_ETM_PKT *) etm_malloc(sizeof(T_ETM_PKT))) == NULL) {
        return ETM_NOMEM;
    }
        
    // Init. of return packet
    pkt->mid    = cid;
    pkt->status = ETM_OK;
    pkt->size   = 0;
    pkt->index  = 0;
       
    switch (cid) {
    case 0x22:
        error = etm_tm3_tm_version_get(pkt, buf);
        break;
    case 0x21:
        error = etm_tm3_tm_mode_set(pkt, buf);
        break;
    case 0x20:
        error = etm_tm3_tm_initialize(pkt, buf);
        break;
    case 0x40: 
        error = etm_tm3_mem_write(pkt, buf, size);
        break;
    case 0x41:
        error = etm_tm3_mem_read(pkt, buf);
        break;
    case 0x42: 
        error = etm_tm3_codec_write(pkt, buf);            
        break;
    case 0x43:
        error = etm_tm3_codec_read(pkt, buf);
        break;
    case 0x44:
        error = etm_tm3_misc_param_write(pkt, buf);
        break;
    case 0x45:
        error = etm_tm3_misc_param_read(pkt, buf);
        break;
    case 0x48:
        error = etm_tm3_misc_enable(pkt, buf);
        break;
  default:
        tr_etm(TgTrCore,"ETM CORE: _tm3core: cid ERROR"); 
        error = ETM_NOSYS;
        break;
    }
    
    if (error > 0) {
        tr_etm(TgTrCore,"ETM CORE: _tm3core: FAILED"); 
        pkt->status = error;
    }
   
    etm_pkt_send(pkt);

    etm_free(pkt); 
    return ETM_OK;
}
