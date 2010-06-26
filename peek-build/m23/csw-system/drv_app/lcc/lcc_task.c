/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian  Lindholm, skl@ti.com
 *
 * Main PWR Task
 * 
 * $Id: pwr_task.c 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/

#include "lcc/lcc.h"
#include "lcc/lcc_task.h"
#include "lcc/lcc_handle_message.h"
#include "lcc/lcc_tm_i.h"
#include "lcc/lcc_trace.h"

#include "rv/rv_defined_swe.h"

#include "ffs/ffs.h"
#include "power/power.h"
#include "chipset.cfg"

#if (ANLG_FAM==11)
#include "abb/bspTwl3029.h"
#include "abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_IntC.h"
#include "abb/bspTwl3029_Madc.h"
#include "abb/bspTwl3029_Bci.h"
#include "abb/bspTwl3029_Power.h"
#endif
#include <string.h>
/******************************************************************************
 * Globals and function prototypes
 ******************************************************************************/

extern T_PWR_CTRL_BLOCK *pwr_ctrl;
extern T_PWR_CFG_BLOCK *pwr_cfg;
	BspTwl3029_MadcResults MadcResults;
	uint8 first_time;
	uint32 int_timer_start;
	uint32 int_timer_current;
	extern uint16 lcc_charger_status;
	extern uint16 lcc_USB_status;
	extern uint8 power_state;
	extern BspTwl3029_MadcChannelId ChannelsToRead;	
	static uint8	lcc_status_value;

extern void pwr_madc_callback(void);

// Event handling functions
	T_RV_RET process_adc_indication             (T_PWR_REQ *request);

// Timer event handling functions
T_RV_RET process_pwr_handle_T1_expiration       (T_PWR_REQ *request);
T_RV_RET process_pwr_handle_T2_expiration       (T_PWR_REQ *request);
T_RV_RET process_pwr_handle_T3_expiration       (T_PWR_REQ *request);
#if(ANLG_FAM!=11)
T_RV_RET process_pwr_handle_T4_expiration       (T_PWR_REQ *request);
#endif

T_RV_RET process_pwr_handle_mmi_info_expiration (T_PWR_REQ *request);

// Interrupt event handling functions
T_RV_RET process_abb_chg_unplugged_ind          (T_PWR_REQ *request);
#if (ANLG_FAM==11)
T_RV_RET process_abb_chg_plugged_ind			(T_PWR_REQ *pwr_request);
#endif

T_RV_RET process_abb_charge_end_ind          (T_PWR_REQ *request);

T_RV_RET process_pwr_WD_overflow (T_PWR_REQ *request);

T_RVM_RETURN pwr_check_files(void);
T_RVM_RETURN pwr_read_files(void);
T_RVM_RETURN pwr_read_chg_files(void);
T_RVM_RETURN pwr_read_cal_files(void);

void build_name(const char *ch_pre, char *cfg_id , UINT8 index, const char * ch_post,  char * name);

	void pwr_assign_default_common();
	void pwr_assign_default_bat();
	void pwr_assign_default_temp();
	void pwr_assign_default_chg();

// FFS function prototypes
effs_t ffs_stat(const char *name, struct stat_s *stat);
int ffs_fread(const char *name, void *addr, int size);

void ttr(unsigned trmask, char *format, ...);
void str(unsigned mask, char *string);

void check_charger_status(void);
uint8 pwr_charge_start_check(void);

#define ENABLE_CI_CV_OR_PWM 1 					//((ENABLE_AC_CI_CV_CHARGING == 1) ||(ENABLE_AC_PWM_CHARGING == 1))

	// Assign with default common data (common.cfg)
	void pwr_assign_default_common()
	{
	    pwr_cfg->common.pins      = LCC_PINS;
	    pwr_cfg->common.chg_dedic = LCC_CHG_DEDIC;
	    pwr_cfg->common.sampling  = LCC_SAMPLING;
	    pwr_cfg->common.mod_cycle = LCC_MOD_CYCLE;
	    pwr_cfg->common.alfa1     = LCC_ALFA1;
	    pwr_cfg->common.alfa2     = LCC_ALFA2;
	    pwr_cfg->common.alfa3     = LCC_ALFA3;
	    pwr_cfg->common.rise_thr  = LCC_RISE_THR;
	}

	// Assign with default battery data (bat.cfg)
	void pwr_assign_default_bat()
	{
	// Use compiled in battery settings
	    pwr_cfg->bat.type          = LCC_BAT_TYPE;
	    pwr_cfg->bat.rf_temp       = LCC_RF_TEMP;
	    pwr_cfg->bat.id_low        = LCC_BAT_ID_LOW;
	    pwr_cfg->bat.id_high       = LCC_BAT_ID_HIGH;
	    pwr_cfg->bat.cbat          = LCC_CBAT;
	    pwr_cfg->bat.ratio         = LCC_RATIO;
	    pwr_cfg->bat.T1            = LCC_T1;
	    pwr_cfg->bat.T2            = LCC_T2;
	    pwr_cfg->bat.T3            = LCC_T3;
	    pwr_cfg->bat.chg_start_thr = LCC_CV_CHG_START_THR;
	    pwr_cfg->bat.chg_stop_thr  = LCC_CV_CHG_STOP_THR;
	    pwr_cfg->bat.chg_ctrl_thr  = LCC_CHG_CTRL_THR;
	    pwr_cfg->bat.chg_again_thr = LCC_CHG_AGAIN_THR;
	}

	// Assign with default temperature data (temp.cfg)
	void pwr_assign_default_temp()
	{
	    // Use compiled in temperature settings
	    pwr_cfg->temp.tbat_min = LCC_TEMP_TBAT_MIN;
	    pwr_cfg->temp.tbat_max = LCC_TEMP_TBAT_MAX;
	    pwr_cfg->temp.a0       = LCC_TEMP_A0;
	    pwr_cfg->temp.a1       = LCC_TEMP_A1;
	    pwr_cfg->temp.a2       = LCC_TEMP_A2;

	    pwr_cfg->temp.cap[0] = LCC_TEMP_CAP_100;
	    pwr_cfg->temp.cap[1] = LCC_TEMP_CAP_90;
	    pwr_cfg->temp.cap[2] = LCC_TEMP_CAP_80;
	    pwr_cfg->temp.cap[3] = LCC_TEMP_CAP_70;
	    pwr_cfg->temp.cap[4] = LCC_TEMP_CAP_60;
	    pwr_cfg->temp.cap[5] = LCC_TEMP_CAP_50;
	    pwr_cfg->temp.cap[6] = LCC_TEMP_CAP_40;
	    pwr_cfg->temp.cap[7] = LCC_TEMP_CAP_30;
	    pwr_cfg->temp.cap[8] = LCC_TEMP_CAP_20;
	    pwr_cfg->temp.cap[9] = LCC_TEMP_CAP_10;

	    pwr_cfg->temp.i_meas1  = LCC_TEMP_I_MEAS1;
	    pwr_cfg->temp.i_meas2  = LCC_TEMP_I_MEAS2;

	    pwr_cfg->temp.v2t_1[0] = LCC_TEMP_V2T_1_MINUS_20;
	    pwr_cfg->temp.v2t_1[1] = LCC_TEMP_V2T_1_MINUS_10;
	    pwr_cfg->temp.v2t_1[2] = LCC_TEMP_V2T_1_0;
	    pwr_cfg->temp.v2t_1[3] = LCC_TEMP_V2T_1_PLUS_10;
	    pwr_cfg->temp.v2t_1[4] = LCC_TEMP_V2T_1_PLUS_20;
	    pwr_cfg->temp.v2t_1[5] = LCC_TEMP_V2T_1_PLUS_30;
	    pwr_cfg->temp.v2t_1[6] = LCC_TEMP_V2T_1_PLUS_40;
	    pwr_cfg->temp.v2t_1[7] = LCC_TEMP_V2T_1_PLUS_50;
	    pwr_cfg->temp.v2t_1[8] = LCC_TEMP_V2T_1_PLUS_60;

	    pwr_cfg->temp.v2t_2[0] = LCC_TEMP_V2T_2_MINUS_20;
	    pwr_cfg->temp.v2t_2[1] = LCC_TEMP_V2T_2_MINUS_10;
	    pwr_cfg->temp.v2t_2[2] = LCC_TEMP_V2T_2_0;
	    pwr_cfg->temp.v2t_2[3] = LCC_TEMP_V2T_2_PLUS_10;
	    pwr_cfg->temp.v2t_2[4] = LCC_TEMP_V2T_2_PLUS_20;
	    pwr_cfg->temp.v2t_2[5] = LCC_TEMP_V2T_2_PLUS_30;
	    pwr_cfg->temp.v2t_2[6] = LCC_TEMP_V2T_2_PLUS_40;
	    pwr_cfg->temp.v2t_2[7] = LCC_TEMP_V2T_2_PLUS_50;
	    pwr_cfg->temp.v2t_2[8] = LCC_TEMP_V2T_2_PLUS_60;

	}

	// Assign with default charger data (chg.cfg)
	void pwr_assign_default_chg()
	{
	    pwr_cfg->chg.type      = LCC_CHG_TYPE;
	    pwr_cfg->chg.ichg_max  = LCC_ICHG_MAX;
	    pwr_cfg->chg.vchg_low  = LCC_VCHG_LOW;
	    pwr_cfg->chg.vchg_high = LCC_VCHG_HIGH;
	}

	/*****************************************************************
	* Function 	: pwr_check_files()
	* Description	:
	*  This function checks the existance of FFS directories and files related
	*  to the PWR module - See RD818
	*  If the existance of the object is MANDATORY pwr_check_files returns an 
	*  error and the PWR configuration is stopped
	*  If the existance of the object is OPTIONAL pwr_check_files a
	*  warning is given and the PWR configuration proceeds
	*******************************************************************/
T_RVM_RETURN pwr_check_files()
{
    T_FFS_SIZE error;
    T_FFS_STAT stat;

    ttw(ttr(TTrInit, "pwr_check_files(%d)" NL, 0));

    // Check directories:
    // /pwr                       MANDATORY
    // /pwr/bat                   MANDATORY
    // /pwr/chg                   OPTIONAL
    // /mmi                       OPTIONAL
    // /mmi/pwr                   OPTIONAL

    // MANDATORY directories
    if ((error = ffs_stat("/pwr", &stat)) == EFFS_OK) {
        if (stat.type != OT_DIR) {
           ttr(TTrFatal, "pwr exists but is not a directory %d" NL, 0);
		   return ((T_RVM_RETURN)EFFS_NOTADIR);

        }
    } else {
        ttr(TTrFatal, "no /pwr directory %d" NL, 0);
        return ((T_RVM_RETURN)error);
    }
    if ((error = ffs_stat("/pwr/bat", &stat)) == EFFS_OK) {
        if (stat.type != OT_DIR) {
           ttr(TTrFatal, "/pwr/bat exists but is not a directory %d" NL, 0);
           return ((T_RVM_RETURN)EFFS_NOTADIR);
        }
    } else {
        ttr(TTrFatal, "no /pwr/bat directory %d" NL, 0);
        return ((T_RVM_RETURN)error);
    }


    // OPTIONAL directories
    if ((error = ffs_stat("/pwr/chg", &stat)) == EFFS_OK) {
        if (stat.type != OT_DIR) {
           ttr(TTrWarning, "/pwr/chg exists but is not a directory %d" NL, 0);
        }
    } else {
        ttr(TTrWarning, "no /pwr/chg directory %d" NL, 0);
    }
    if ((error = ffs_stat("/mmi", &stat)) == EFFS_OK) {
        if (stat.type != OT_DIR) {
           ttr(TTrWarning, "/mmi exists but is not a directory %d" NL, 0);
        }
    } else {
        ttr(TTrWarning, "no /mmi directory %d" NL, 0);
    }
    if ((error = ffs_stat("/mmi/pwr", &stat)) == EFFS_OK) {
        if (stat.type != OT_DIR) {
           ttr(TTrWarning, "/mmi/pwr exists but is not a directory %d" NL, 0);
        }
    } else {
        ttr(TTrWarning, "no /mmi/pwr directory %d" NL, 0);
    }

    // Check calibration files:
    // /pwr/vbat.cal              MANDATORY
    // NOT checked - it MUST be present - else we will have no Vbat measurements

    // Check configuration files:
    // /pwr/common.cfg            MANDATORY
    // /pwr/bat/bat<N>.cfg        MANDATORY
    // /pwr/bat/temp<N>.cfg       OPTIONAL
    // /pwr/chg/chg<N>.cfg        OPTIONAL


    // MANDATORY files
    if ((error = ffs_stat("/pwr/common.cfg", &stat)) == EFFS_OK) {
        if (stat.type != OT_FILE) {
           ttr(TTrFatal, "/pwr/common.cfg exists but is not a file %d" NL, 0);
            return ((T_RVM_RETURN)EFFS_NOTADIR);

        }
    } else {
        ttr(TTrFatal, "no /pwr/common.cfg file %d" NL, 0);
        return ((T_RVM_RETURN)error);
    }

    ttw(ttr(TTrInit, "pwr_check_files(%d)" NL, 0xFF));
    return RV_OK;
}

	/*****************************************************************
	* Function 	: pwr_check_files()
	* Description	:
	*   This function reads the FFS pwr configuration files
	*                               /pwr/vbat.cal          MANDATORY
	*                               /mmi/pwr/bsie          OPTIONAL
	*                               /pwr/common.cfg        MANDATORY
	*                               /pwr/bat/bat<n>.cfg    MANDATORY
	*                               /pwr/bat/temp<n>.cfg   MANDATORY
	* 	Precondition: Files have been checked with pwr_check_files()
	*                     Therefore we know they exist. Charger files are read later.
	*******************************************************************/
T_RVM_RETURN pwr_read_files()
{
    T_FFS_SIZE error;
    T_FFS_STAT stat;
    char name[20];
	uint8 i=0,
		load_default_bat_cfg=1, 
	  	load_default_temp_cfg=1; // Load default configuration if  files didn't exist OR the battery id's didn't match;
    char cfg_id;

    ttw(ttr(TTrInit, "pwr_read_files(%d)" NL, 0));


    // Brute force of charger configuration ? /pwr/chg/force
    if ((error = ffs_stat("/pwr/chg/force", &stat)) == EFFS_OK) {
        error = ffs_fread("/pwr/chg/force", &pwr_cfg->data.cforce, 1);
        ttw(ttr(TTrInitLow, "Read /pwr/chg/force(%d)" NL, error));
        pwr_cfg->data.chg_cfg_id = pwr_cfg->data.cforce + '0';
        pwr_ctrl->chg_cfg_id     = pwr_cfg->data.cforce;
    } else {
        // Use 'normal' 'plug & play' configuration
        pwr_cfg->data.cforce = 0;
        pwr_cfg->data.chg_cfg_id = 1 + '0'; // Default
        pwr_ctrl->chg_cfg_id     = 1;       // Default
    }

    // Brute force of battery configuration ? /pwr/bat/force
    if ((error = ffs_stat("/pwr/bat/force", &stat)) == EFFS_OK) {
        error = ffs_fread("/pwr/bat/force", &pwr_cfg->data.bforce, 1);
        ttw(ttr(TTrInitLow, "Read /pwr/bat/force(%d)" NL, error));
        pwr_ctrl->cfg_id     = pwr_cfg->data.bforce;
        pwr_cfg->data.cfg_id = pwr_cfg->data.bforce + '0';
    } else {
        // Use 'normal' 'plug & play' configuration
        // Precondition: We have a reliable battery id measurement
        pwr_cfg->data.bforce = 0;
        ttw(ttr(TTrInitLow, "Plug & play bat_id=%d" NL, pwr_cfg->data.bat_id));
    }

    // Read /pwr/common.cfg
    error = ffs_fread("/pwr/common.cfg", &pwr_cfg->common, PWR_COMMON_CFG_SIZE);
	if (error < 0) 
    	{
      		// Use default compiled in settings 
      		// common.cfg
      		ttr(TTrWarning, "No common.cfg (%d) -using defaults" NL, error);
      		pwr_assign_default_common();
      
    	} 
	else
    	{
    ttw(ttr(TTrInitLow, "Read /pwr/common.cfg(%d)" NL, error));
    	}

    // Read /mmi/pwr/bsie 
    // Apply defaults if file doesn't exist
    if ((error = ffs_stat("/mmi/pwr/bsie.cfg", &stat)) == EFFS_OK) {
        if (stat.type != OT_FILE) {
           ttr(TTrWarning, "/mmi/pwr/bsie.cfg exists but is not a file %d" NL, 0);
           return ((T_RVM_RETURN)EFFS_NOTAFILE);

        } else {
           error = ffs_fread("/mmi/pwr/bsie.cfg", &pwr_cfg->mmi, sizeof(pwr_cfg->mmi));
           ttw(ttr(TTrInitLow, "Read /mmi/pwr/bsie.cfg(%d)" NL, error));
        }
    } else {
        ttr(TTrWarning, "no /mmi/pwr/bsie file %d" NL, 0);
        // Apply defaults
        pwr_cfg->mmi.repetition = PWR_MMI_REP_THR;
    }

    if (pwr_cfg->data.bforce > 0) {
        // Brute force battery configuration
        build_name("/pwr/bat/bat", &pwr_cfg->data.cfg_id, 12, ".cfg", name);
        error = ffs_fread(name, &pwr_cfg->bat, PWR_BAT_CFG_SIZE);
	if (error > 0) 
		load_default_bat_cfg = 0;
	
	build_name("/pwr/bat/temp", &pwr_cfg->data.cfg_id, 13, ".cfg", name);
	error = ffs_fread(name, &pwr_cfg->temp, PWR_TEMP_CFG_SIZE);
	if (error > 0) 
		load_default_temp_cfg = 0;
	
    } else {
        // Find out which <n> and read /pwr/bat/bat<n>.cfg
        // We know that at least one battery configuration file exists
        // Pick the file that matches the bat_id measured earlier

        for (i = 1; i <= 5; i++) {
            cfg_id = i + '0';
            build_name("/pwr/bat/bat", &cfg_id, 12, ".cfg", name);
            error = ffs_fread(name, &pwr_cfg->bat, PWR_BAT_CFG_SIZE);
            // Found the right battery id??
            if ((pwr_cfg->data.bat_id >= pwr_cfg->bat.id_low) &&
                (pwr_cfg->data.bat_id <= pwr_cfg->bat.id_high)) {
                ttw(ttr(TTrInitLow, "Chose %s" NL, name));
                // Save the configuration number in the name
                pwr_ctrl->cfg_id = i;
                pwr_cfg->data.cfg_id = cfg_id;
                pwr_ctrl->flag_bat_unknown = 0;

            /* DONT LOAD default battery cfg if configuration file is present. */
            if (error > 0) 
               load_default_bat_cfg = 0;

                // Read the corresponding temperature configuration
                build_name("/pwr/bat/temp", &pwr_cfg->data.cfg_id, 13, ".cfg", name);
                error = ffs_fread(name, &pwr_cfg->temp, PWR_TEMP_CFG_SIZE);
		if (error > 0) 
                        // DONT LOAD default temperature configuration
		        load_default_temp_cfg = 0;	
                break;
            }
        }

        // Check if a matching battery configuration was found
        if ((pwr_cfg->data.cfg_id < '1') || (pwr_cfg->data.cfg_id > '5')) {
            pwr_cfg->data.cfg_id = '1';
            pwr_ctrl->cfg_id = 1;
            ttr(TTrWarning, "No matching battery configuration id - Defaults to %d" NL, pwr_ctrl->cfg_id);

            // Flag that battery configuration was unknown
            // Inform the MMI later when it has registered
            pwr_ctrl->flag_bat_unknown = 1;
        }
    }
	// Apply default values - if no files or relevant Id's were found
    	if (load_default_bat_cfg == 1)
    		{
			ttr(TTrWarning, "No bat.cfg (%d) OR no matching battery configuration -using defaults" NL, 0);
			pwr_assign_default_bat();
    		}		
    	if (load_default_temp_cfg == 1)
    		{
			ttr(TTrWarning, "No temp.cfg (%d) OR no matching battery configuration -using defaults" NL, 0);
			pwr_assign_default_temp();
    		}	
    ttw(ttr(TTrInit, "pwr_read_files(%d)" NL, 0xFF));
    return RV_OK;
}

	/*****************************************************************
	* Function 	: pwr_read_cal_files()
	* Description	:
	*    Read calibration files only 
	*
	*******************************************************************/

T_RVM_RETURN pwr_read_cal_files()
{
    T_FFS_SIZE error;

    // Read /pwr/vbat.cal
    error = ffs_fread("/pwr/vbat.cal", &pwr_cfg->cal.vbat, sizeof(pwr_cfg->cal.vbat));
	if ( error < EFFS_OK ) 
    	{
        	// No calibration file found - use default values - VALUES MUST BE CALCULATED!!
        	// Overwrite from ETM with 'pww 11 <alfa_num> <alfa_denom> <beta>'
		ttr(TTrWarning, "No vbat.cal (%d) -using defaults" NL, error);
		pwr_cfg->cal.vbat.alfa_num   = LCC_ALFA_NUM;
        	pwr_cfg->cal.vbat.alfa_denom = LCC_ALFA_DENOM;
        	pwr_cfg->cal.vbat.beta       = LCC_BETA;
	}
    ttw(ttr(TTrInitLow, "Read /pwr/vbat.cal(%d)" NL, error));
    ttw(ttr(TTrInitLow, "pwr_cfg->cal.vbat.alfa_num=%d" NL, pwr_cfg->cal.vbat.alfa_num));
    ttw(ttr(TTrInitLow, "pwr_cfg->cal.vbat.alfa_denom=%d" NL, pwr_cfg->cal.vbat.alfa_denom));
    ttw(ttr(TTrInitLow, "pwr_cfg->cal.vbat.beta=%d" NL, pwr_cfg->cal.vbat.beta));

	return RV_OK;
}

	/*****************************************************************
	* Function 	: pwr_check_files()
	* Description	:
	*     This function reads the FFS pwr configuration file
	*	It is invoked when a charger is plugged
	*	                               /pwr/chg/chg<n>.cfg    MANDATORY
	*******************************************************************/
T_RVM_RETURN pwr_read_chg_files()
{
    T_FFS_SIZE error;
    char name[20];
	uint8 i=0;
    char chg_id;
   uint8 load_default_chg_cfg=1; 

    ttw(ttr(TTrInit, "pwr_read_chg_files(%d)" NL, 0));

    if (pwr_cfg->data.cforce > 0) {
        // Brute force charger configuration
        build_name("/pwr/chg/chg", &pwr_cfg->data.chg_cfg_id, 12, ".cfg", name);
        error = ffs_fread(name, &pwr_cfg->chg, PWR_CHG_CFG_SIZE);
        if(error > 0)
  	  	load_default_chg_cfg=0; 
		
        ttw(ttr(TTrInitLow,"error = %d" NL, error));

        // Readout /pwr/chg/chg<N>.cfg
        ttw(ttr(TTrInitLow,"chg.cfg: type: %d" NL, pwr_cfg->chg.type));
        ttw(ttr(TTrInitLow,"ichg_max: %d" NL, pwr_cfg->chg.ichg_max));
        ttw(ttr(TTrInitLow,"vchg_low: %d" NL, pwr_cfg->chg.vchg_low));
        ttw(ttr(TTrInitLow,"vchg_high: %d" NL, pwr_cfg->chg.vchg_high));
    } else {
        // Find out which <n> and read /pwr/chg/chg<n>.cfg
        // We know that at least one charger configuration file exists
        // Pick the file that matches the chg_id measured earlier

        for (i = 1; i <= 5; i++) {
            chg_id = i + '0';
            build_name("/pwr/chg/chg", &chg_id, 12, ".cfg", name);
            error = ffs_fread(name, &pwr_cfg->chg, PWR_CHG_CFG_SIZE);
            if(error > 0)
      	  	load_default_chg_cfg=0;       	  
			
            ttw(ttr(TTrInitLow,"error = %d" NL, error));

            // Readout /pwr/chg/chg<N>.cfg
            ttw(ttr(TTrInitLow,"chg.cfg: type: %d" NL, pwr_cfg->chg.type));
            ttw(ttr(TTrInitLow,"ichg_max: %d" NL, pwr_cfg->chg.ichg_max));
            ttw(ttr(TTrInitLow,"vchg_low: %d" NL, pwr_cfg->chg.vchg_low));
            ttw(ttr(TTrInitLow,"vchg_high: %d" NL, pwr_cfg->chg.vchg_high));

            // Found the right charger id??
            if ((pwr_cfg->data.chg_id > pwr_cfg->chg.vchg_low) &&
                (pwr_cfg->data.chg_id < pwr_cfg->chg.vchg_high)) {
                ttw(ttr(TTrInitLow, "Chose %s" NL, name));
                // Save the configuration number in the name
                pwr_ctrl->chg_cfg_id = i;
                pwr_cfg->data.chg_cfg_id = chg_id;
                pwr_ctrl->flag_chg_unknown = 0;

                break;
            }
        }

        // Check if a matching charger configuration was found
        if ((pwr_cfg->data.chg_cfg_id < '1') || (pwr_cfg->data.chg_cfg_id > '5')) {
            pwr_cfg->data.chg_cfg_id = '1';
            pwr_ctrl->chg_cfg_id = 1;
            // Flag that charger configuration was unknown
            // Inform the MMI later when it has registered
            pwr_ctrl->flag_chg_unknown = 1;
        }
    }

         /* Use compiled in settings */
         if(load_default_chg_cfg==1)
         {
     		ttr(TTrWarning, "No chg.cfg (%d) OR no matching charger configuration -using defaults" NL, 0);
           
            pwr_assign_default_chg();    	
         }
	
    ttw(ttr(TTrInit, "pwr_read_chg_files(%d)" NL, 0xFF));
    return ((T_RVM_RETURN)error);
}

void *pwr_malloc(int size)
{
    void *addr;

    if (rvf_get_buf(pwr_ctrl->prim_id, size, &addr) == RVF_RED) {
        ttr(TTrFatal, "PWR FATAL: No Memory (%d)" NL, pwr_ctrl->state);
        return NULL;
    }

    return addr;
}

void pwr_free(void *addr)
{
	int error =0;

    ttw(ttr(TTrEvent, "pwr_free (%d)" NL, 0));
    if ((error = rvf_free_buf(addr)) != RV_OK) {
        ttr(TTrFatal, "PWR FATAL: pwr_free (%d)" NL, error);
    }
}

#if (ANLG_FAM==11)
void lcc_callback_handler(uint8 Id)
{
struct pwr_adc_ind_s *addr;
switch(Id)   
{
case     (BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS&0xFF):
		{
			if(lcc_status_value & (1<<BSP_TWL3029_POWER_USB_OFFSET))
			      {
					lcc_USB_status = 1;
				}
				else
				{
					lcc_USB_status = 0;
					
				}
			break;
		}
case	(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER&0xFF):
		{
			if (lcc_status_value & (1<<BSP_TWL3029_POWER_CHG_OFFSET))   {
			    lcc_charger_status = 1;	
         		rvf_send_trace("IQ EXT: Charger Plug",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
			    // Forward charger plug indication to LCC task
                // NOTE that memory is allocated externally in the LCC task
          				if (rvf_get_buf(pwr_ctrl->prim_id, sizeof(struct pwr_req_s), (void *)&addr) == RVF_RED) 
					{
              		rvf_send_trace("rvf_get_buf failed#1",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
              		rvf_dump_mem();
					break;
          		}
          		addr->header.msg_id        = PWR_CHARGER_PLUGGED_IND;
          		addr->header.src_addr_id   = (T_RVF_ADDR_ID)NULL_PARAM; /* FIXME*/
          		addr->header.dest_addr_id  = pwr_ctrl->addr_id;
          		addr->header.callback_func = NULL;
          				if (rvf_send_msg(pwr_ctrl->addr_id, addr) != RV_OK)	{
                       		rvf_send_trace("SPI FATAL: Send failed!",23, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
          		}
	         }
      	   else   {
		   	  lcc_charger_status = 0;
         		rvf_send_trace("IQ EXT: Charger Unplug",22, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
					// Forward charger unplug indication to LCC task
          				// NOTE that memory is allocated externally in the LCC task
          				if (rvf_get_buf(pwr_ctrl->prim_id, sizeof(struct pwr_req_s), (void *)&addr) == RVF_RED) 
					{
              			rvf_send_trace("rvf_get_buf failed#2",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
              			rvf_dump_mem();
						break;
          			}
          			addr->header.msg_id        = PWR_CHARGER_UNPLUGGED_IND;
          			addr->header.src_addr_id   = (T_RVF_ADDR_ID)NULL_PARAM; /* FIXME */
          			addr->header.dest_addr_id  = pwr_ctrl->addr_id;
          			addr->header.callback_func = NULL;
          				if (rvf_send_msg(pwr_ctrl->addr_id, addr) != RV_OK)  {
              			rvf_send_trace("ABB FATAL: Send failed!",23, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
      			}
   			}
          break; 
		} 
}

}
	
void pwr_abb_interrupt_handler(BspTwl3029_IntC_SourceId id)
{
	//uint8	status_value;
	struct pwr_adc_ind_s *addr;
	switch(id) 
	{
		#if ENABLE_USB_CHARGING
		case     BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS:
				 {
				  bspTwl3029_Power_SwitchOnStatusWithCallbackval(&lcc_status_value,lcc_callback_handler,
				  				(BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS&0xFF));
			      }
			      break;			
		#endif 	//end of ENABLE_USB_CHARGING	  
		case     BSP_TWL3029_INTC_SOURCE_ID_BCI_CHARGE_STOP:
				rvf_send_trace("IQ EXT: Charge End",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
			    	// Forward charger plug indication to LCC task
                		// NOTE that memory is allocated externally in the LCC task
          			if (rvf_get_buf(pwr_ctrl->prim_id, sizeof(struct pwr_req_s), (void *)&addr) == RVF_RED) 
				{
              			rvf_send_trace("rvf_get_buf failed#1",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
              			rvf_dump_mem();
					break;
          			}
          			addr->header.msg_id        = PWR_CHARGE_END_IND;
          			addr->header.src_addr_id   = (T_RVF_ADDR_ID)NULL_PARAM; /* FIXME*/
          			addr->header.dest_addr_id  = pwr_ctrl->addr_id;
          			addr->header.callback_func = NULL;
          			if (rvf_send_msg(pwr_ctrl->addr_id, addr) != RV_OK) 
				{
              			rvf_send_trace("SPI FATAL: Send failed!",23, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
          			}
				break;		
		#if ENABLE_CI_CV_OR_PWM
		case     BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER:
			{
			/* Charger plug IN or OUT */
			bspTwl3029_Power_SwitchOnStatusWithCallbackval(&lcc_status_value,lcc_callback_handler,
							(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER&0xFF));
			}
 			break;
		#endif 	 //end of ENABLE_CI_CV_OR_PWM 
			case     BSP_TWL3029_INTC_SOURCE_ID_WD_OVERFLOW:
			
			rvf_send_trace("IQ EXT: WD_INT",14,NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, SPI_USE_ID);
				if (rvf_get_buf(pwr_ctrl->prim_id, sizeof(struct pwr_req_s), (void *)&addr) == RVF_RED) 
				{
              			rvf_send_trace("rvf_get_buf failed#2",20, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
              			rvf_dump_mem();
						break;
          			}
          			addr->header.msg_id        = PWR_WD_OVERFLOW_IND;
          			addr->header.src_addr_id   = (T_RVF_ADDR_ID)NULL_PARAM;
          			addr->header.dest_addr_id  = pwr_ctrl->addr_id;
          			addr->header.callback_func = NULL;
          			if (rvf_send_msg(pwr_ctrl->addr_id, addr) != RV_OK)
				{
              			rvf_send_trace("ABB FATAL: Send failed!",23, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, LCC_USE_ID);
      				}
			break;
		case     BSP_TWL3029_INTC_SOURCE_ID_BATT_LOW:
		default:
			break;
	}
	
}
#endif

	/*****************************************************************
	* Function 	: check_charger_status
	* Description	:
	*   				checks the presents of AC charger and USB charger by reading 
	*				a Hardware status register in Triton
	*
	*******************************************************************/
void check_charger_status(void)
{
	uint8	status_value = 0;
	
#if ENABLE_CI_CV_OR_PWM
	pwr_abb_interrupt_handler(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER);
#endif //end of ENABLE_CI_CV_OR_PWM 

#if ENABLE_USB_CHARGING
	if(status_value & (1<<BSP_TWL3029_POWER_USB_OFFSET))
	{
	lcc_USB_status = 1;
	} 
#endif  //end of ENABLE_USB_CHARGING
	}

	/*****************************************************************
	* Function 	: check_charge_start_check
	* Description	:
	*   				checks whether the charge start conditions are reached or not.
	*Parameters	:
	*				None
	*Return		:
	*		1: charging needs to be started		
	*		0: charging need not be started 
	*******************************************************************/
uint8 pwr_charge_start_check(void)
{
	if(pwr_ctrl->state == SUP)
	{
#if ENABLE_CI_CV_OR_PWM
	if((lcc_charger_status == 1)&&(pwr_check_topping() == TRUE))
		{
			return(1);
		}
		else
		{
		#if(ENABLE_USB_CHARGING == 0)
		return(0);
		#endif //end of ENABLE_USB_CHARGING
	}
#endif  //end of ENABLE_CI_CV_OR_PWM 

#if ENABLE_USB_CHARGING
	if((lcc_USB_status == 1)&&(pwr_ctrl->capacity < pwr_cfg->bat.chg_again_thr))
	{
		return(1);
	}
	else
	{
		return(0);
	}	
#endif  //end of ENABLE_USB_CHARGING
	}
	else   {
		return((uint8)(-1));
	} 
	}

	/*****************************************************************
	* Function 	: 	pwr_task
	* Description	:
	*   				called from pei_run.
			here the task waits for a message.
			if a message is posted, a curresponding function will be called to process it.
	*Parameters	:
	*				None
	*Return		:
	*				None 
	*******************************************************************/
void pwr_task()
{

    void                 *request;
    struct pwr_req_s     *pwr_request;
	int    error =0;
#if (ANLG_FAM==11)
	ChannelsToRead = ( BSP_TWL3029_ADC_CHANNEL_4_BATT_TYPE   | \
					      BSP_TWL3029_ADC_CHANNEL_5_BTEMP |\
					      BSP_TWL3029_ADC_CHANNEL_6_USBVBUS |\
					      BSP_TWL3029_ADC_CHANNEL_8_ICHG |\
					      BSP_TWL3029_ADC_CHANNEL_9_VCHG |\
					      BSP_TWL3029_ADC_CHANNEL_10_VBAT ); 
					      
	//ChannelsToRead = 0x3B8; // Enable only 6 Channels : Battery Type,Charging Current,USBVBUS,Charger Voltage,VBAT,BTEMP.
	bspTwl3029_Madc_enableRt(NULL,ChannelsToRead, pwr_madc_callback, &MadcResults);
	#endif
	
    ttw(ttr(TTrEnv, "pwr_task(%d)" NL, 0));
	
	check_charger_status();   //check whether the charger is plugged or not and update the charger status global variable
	
    while (1) {
        rvf_wait(RVF_TASK_MBOX_0_EVT_MASK,0);
        request  = rvf_read_mbox(RVF_TASK_MBOX_0);
        pwr_request = (struct pwr_req_s *)request;

        if (request != NULL) {

            ttw(ttr(TTrEvent,    "Received Event(%d)" NL, pwr_request->header.msg_id));
            ttw(ttr(TTrEventLow, "src_addr_id(%d)" NL, pwr_request->header.src_addr_id));
            ttw(ttr(TTrEventLow, "dest_addr_id(%d)" NL, pwr_request->header.dest_addr_id));

            switch (pwr_request->header.msg_id) {
#if ENABLE_CI_CV_OR_PWM
            case PWR_CHARGER_PLUGGED_IND:
				#if (ANLG_FAM!=11)
                // Sometimes (low voltage - init) receiving a ghost charger plug although interrupts are disabled
                pwr_free(request);
				#else
				error = process_abb_chg_plugged_ind(pwr_request);
				#endif
                break;
            case PWR_CHARGER_UNPLUGGED_IND:
                error = process_abb_chg_unplugged_ind(pwr_request);
                break;
#endif  //end of ENABLE_CI_CV_OR_PWM 
#if ENABLE_USB_CHARGING
	     case PWR_USB_PLUGGED_IND:
	  //error = process_abb_USB_plugged_ind(pwr_request);
		  break;
	     case PWR_USB_UNPLUGGED_IND:
	  //error = process_abb_USB_unplugged_ind(pwr_request);
		  break;
#endif 	//end of ENABLE_USB_CHARGING  
            case PWR_ADC_IND :
	        error = process_adc_indication(request);
                break;
	     case PWR_CHARGE_END_IND :
		  error = process_abb_charge_end_ind(request); 
		  break;	
            // Timers

            case TIMER_T1_EXPIRED:
                error = process_pwr_handle_T1_expiration(request);
                break;
            case TIMER_T2_EXPIRED:
                error = process_pwr_handle_T2_expiration(request);
                break;
           case TIMER_T3_EXPIRED:
                error = process_pwr_handle_T3_expiration(request);
                break;
				
	    #if(ANLG_FAM!=11)
	    case TIMER_T4_EXPIRED:
                error = process_pwr_handle_T4_expiration(request);
                break;
	    #endif 		
		   
	    case TIMER_MMI_INFO_EXPIRED:
                error = process_pwr_handle_mmi_info_expiration(request);
                break;
	    case PWR_WD_OVERFLOW_IND:
	  //error = process_pwr_WD_overflow(request);
		  break;
            default :
                {
                ttr(TTrFatal, "PWR FATAL: Unknown Event: %d" NL, pwr_request->header.msg_id);
                ttr(TTrFatal, "                   State: %d" NL, pwr_ctrl->state);
                // Exception Handling - Unknown Event
                }
            }
        } else {
            // Exception Handling - NULL pointer
            ttr(TTrFatal, "PWR FATAL: NULL pointer (%d)" NL, pwr_ctrl->state);
        }
    }
}
