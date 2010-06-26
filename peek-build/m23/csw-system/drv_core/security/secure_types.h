/*                %Z% nom : %M% SID: %I% date : %G%                           */
/* Filename:      %M%                                                         */
/* Version:       %I%                                                         */
/******************************************************************************
 *                   WIRELESS COMMUNICATION SYSTEM DEVELOPMENT
 *
 *             (C) 2002 Texas Instruments France. All rights reserved
 *
 *                          Author : Constantin HAIDAMOUS
 *
 *
 *  Important Note
 *  --------------
 *
 *  This S/W is a preliminary version. It contains information on a product 
 *  under development and is issued for evaluation purposes only. Features 
 *  characteristics, data and other information are subject to change.
 *
 *  The S/W is furnished under Non Disclosure Agreement and may be used or
 *  copied only in accordance with the terms of the agreement. It is an offence
 *  to copy the software in any way except as specifically set out in the 
 *  agreement. No part of this document may be reproduced or transmitted in any
 *  form or by any means, electronic or mechanical, including photocopying and
 *  recording, for any purpose without the express written permission of Texas
 *  Instruments Inc.
 *
 ******************************************************************************
 *
 *  FILE NAME: secure_types.h
 *
 *
 *  PURPOSE:
 *            
 *
 *  FILE REFERENCES:
 *
 *  Name                  IO      Description
 *  -------------         --      ---------------------------------------------
 *  
 *
 *
 *  EXTERNAL VARIABLES:
 *
 *  Source: none
 *
 *  Name                    Type              IO   Description
 *  -------------------     ---------------   --   ----------------------------
 *
 *
 *
 *  ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
 *  
 *
 *
 *  ASSUMPTION, CONSTRAINTS, RESTRICTIONS:
 *  
 *
 *
 *  NOTES:
 *  
 *
 *
 *  REQUIREMENTS/FUNCTIONAL SPECIFICATION REFERENCES:
 *
 *
 *
 *
 *  DEVELOPMENT HISTORY:
 *
 *  Date        Name(s)               Version  Description
 *  ----------  --------------------  -------  ---------------------------------
 *  04/12/2002  Constantin HAIDAMOUS  V1.0.0   First release
 *  22-Apr-02   Francois AMAND        V1.0.1   Update of secure status to
 *                                             facilitate integration in final
 *                                             ROM code.
 *                                             Data alignment in struture to
 *                                             optimize memory requirement.
 *                                             Management of global secure data.
 *  28-Apr-02   Francois AMAND        V1.0.2   Update some constants to be
 *                                             compliant with certificate
 *                                             definition.
 *                                             Add CALYPSO PLUS specific
 *                                             parameters structure.
 *                                             Differentiate Manufacturer and
 *                                             Platform certificate.
 *                                             Add debug request field in Manu-
 *                                             facturer certificate.
 *                                             Optimize certificate field size.
 *                                             Change commentary wrapper to be
 *                                             ANSI compliant.
 *  29-Apr-02   Francois AMAND        V1.0.3   Remove unused constant.
 *                                             Set under compilation flag the
 *                                             secure services.
 *  14-May-02   Francois AMAND        V1.0.4   Add CONF_CSI field in T_CONF_PARAM
 *                                             structure definition.
 *                                             Change name of CS_MODE to EX_CTRL
 *                                             Change size of DCCTRL_CSx to 8
 *                                             bits to optimize size and alignment.
 *  17-May-02   Francois AMAND        V1.0.5   Modification of hashing size for
 *                                             binding/unbinding.
 *  17-May-02   Constantin HAIDAMOUS  V1.0.6   Integration of data structure for
 *                                             binding/unbinding services API .
 *  17-May-02   Francois AMAND        V1.0.7   Integration of RSA S/W and Check 
 *                                             ROM in Secure Services.
 *                                             Update of Secure Services manage-
 *                                             ment.
 *  22-May-02   Constantin HAIDAMOUS  V1.0.8   Changing RSA modulus length , Max
 *                                             RSA length =2048 bits. Adding initial
 *                                             vectore for Binding structure.
 *  27-May-02   Francois AMAND        V1.0.9   Changing SAMSON to CALYPSO.
 *                                             Addition of Secure Services for
 *                                             RUN Time checker.
 *  28-May-02   Constantin HAIDAMOUS  V1.0.10  Changing C_PLAT_SIG_SIZE to 4.
 *
 *  06-June-02  Constantin HAIDAMOUS  V1.0.11  Changing Secure service API.
 * 
 *  12-Jun-02   Francois AMAND        V1.0.12  Update of T_CONF_PARAM according
 *                                             to last memory interface spec.
 *  13-Jun-02   Constantin HAIDAMOUS  V1.0.13  Update of T_UNBINDCTX for segmented
 *                                             unbind operation optimization.
 *  14-Jun-02   Constantin HAIDAMOUS  V1.0.14  Update of T_BINDCTX for segmented
 *                                             bind operation .
 *  14-Jun-02   Francois AMAND        V1.0.15  Force d_temp in Secure RAM.
 *                                             Remove other definitions.
 *  25-Jun-02   Francois AMAND        V1.0.16  Addition of Platform unbinding
 *                                             during boot concept under compi-
 *                                             lation flag C_PLATFORM_UNBIND_BOOT.
 *  27-Jun-02   Constantin HAIDAMOUS  V1.0.17  Adding C_SECURE_RNG_ALARM to E_SECURE_STATUS
 *                                             C_SECURE_RNG_ALARM = C_RNG_ALARM from Safenet 
 *  09-Jul-02   Constantin HAIDAMOUS  V1.0.18  Adding Application ID and Timeout for Bind function
 *  16-Jul-02   Francois AMAND        V1.0.19  Add S/W pseudo-random generator.
 *  19-Jul-02   Francois AMAND        V1.0.20  Remove S/W pseudo-random.
 *  01-Aug-02   Constantin HAIDAMOUS  V1.0.21  Removing boolean b_HashDone in Bind and Unbind Context 
 *  27-Jan-03   Francois AMAND        V2.0.0   Remove of C_PLATFORM_UNBIND_BOOT flag
 *  27-Jan-03   Francois AMAND        V2.0.1   Add compliance with SW@P 2.0 (REQ03047)
 *  20-Mar-03   Francois AMAND        V2.0.2   Code cleaning
 *  25-Nov-03   Francois AMAND        V2.0.3   Management of TI compiler 3.00.
 *                                             Addition of compilation flag __TMS470__
 *                                             Warning correction with DATA_SECTION
 *  10-Dec-03   Francois AMAND        V2.0.4   Remove of CALYPSO references
 *  28-Jan-05   ArunKumar             V3.0.0   Upgraded for Locosto
 *                                             - Die ID size Change
 *                                             - Platform Data size changed
 *                                             - Because of new EMIF, Conf_param is changed
 *                                             - Additon of T_GPDEVICE_CERTIFICATE as Locosto
 *                                               supports prph. booting for GP Device as well
 *                                             - Because of above feature addition of 
 *                                               E_DEVICE_TYPE
 *  ALGORITHM:
 *
 *
 *******************************************************************************/


#ifdef _PC_RAM_LOADER_
  #include "standard.h"
#else
  #include "global_types.h"
#endif

#ifndef _SECURE_TYPES_H
  #define _SECURE_TYPES_H

  #ifndef C_SECURE_SERVICES
    #define C_SECURE_SERVICES 1
  #endif
  

  /****************************************************************************
   * Constants
   ****************************************************************************/

  #define C_WORD32LGB     4
     
  #ifndef NULL  /* To avoid conflict with other compilers */
    #define NULL (UWORD32 *) 0x00000000L
  #endif

  /*Certificate block*/ 

  /*Length in long word (32 bits)*/

  #define C_SHA1HASHLG          5
  #define C_MD5HASHLG           4
  
  #define C_RSAKEYLG            64 /* Max Modulus size : 2048 bits */
  #define C_RSASIGLG            C_RSAKEYLG
  
#if C_CHIPSET_FLAG == C_CHIPSET_CALYPSOPLUS 
  #define C_DIE_ID_SIZE         2  /* DIE ID defined on 64-bits */
#else
  #define C_DIE_ID_SIZE         4  /* DIE ID defined on 128-bits */ 
#endif
  
#if C_CHIPSET_FLAG == C_CHIPSET_CALYPSOPLUS
  #define C_PLATFORM_DATA_SIZE  16048 /* Maximum value to have CertSize coded on 16-bits */
#else
  #define C_PLATFORM_DATA_SIZE  16044 /* Maximum value to have CertSize coded on 16-bits */
#endif 
  
  #if C_SECURE_SERVICES == 1
    #define C_IVLG              2
    #define C_TDESKEYLG         4
    #define C_TDESKEYLGB        C_TDESKEYLG * C_WORD32LGB
    #define C_PLAT_SIG_SIZE     4 /* Must be a multiple of 64 bits */
  #endif

  #define C_MANUF_SIG_SIZE      C_RSASIGLG

  #define C_FIRM_HASH_SIZE      C_SHA1HASHLG /* Hash of the firmware for GP Device is SHA1 based and hence 160 bits i.e. 20 bytes*/

  /*Certificate Type*/ 

  #define C_CERTTYPE_MAN          0x00
  #define C_CERTTYPE_PLAT         0x01

  #if C_SECURE_SERVICES == 1
    /*Confidentiality request*/ 

    #define C_CRYPTVOID       0x00
    #define C_CRYPTNOREQUEST  0x00
    #define C_CRYPTREQUEST    0x01
  #endif


  /*Debug request*/ 

  #define C_DEBUGNOREQUEST  0x00
  #define C_DEBUGREQUEST    0x01


  /*CS image check request*/ 

  #define C_CSIMGNOREQUEST  0x00
  #define C_CSIMGREQUEST    0x01


  /*Temporary block*/

  #define C_TEMPDATALG        32
  #define C_TEMPDATALGB       C_TEMPDATALG * C_WORD32LGB

  #define C_CERT_TEMP_SWKEY   0
  #define C_CERT_TEMP_DATA    C_CERT_TEMP_SWKEY + C_TDESKEYLG


  /****************************************************************************
   * Structures
   ****************************************************************************/

#if C_CHIPSET_FLAG == C_CHIPSET_CALYPSOPLUS
  /* CALYPSO PLUS Parameters */
  typedef struct {
    /* External Memory Interface configuration */
    UWORD16 d_conf_cs5;
    UWORD16 d_exws_cs5;
    UWORD16 d_ex_ctrl;
  
    /* Authentication configuration */
    UWORD16 d_cs_img_req;
    UWORD32 d_flash_size;
    UWORD32 d_granularity;
  } T_CONF_PARAM;
#else
  /* LOCOSTO Parameters */
  typedef struct {
    /* External Memory Interface configuration */
    UWORD32 emif_conf_reg_cs3;
    UWORD16 emif_conf_reg;    
    UWORD16 emif_adv_conf_reg_cs3;
    UWORD16 emif_dyn_wsr;
    
    /* Authentication configuration */
    UWORD16 d_cs_img_req;
    UWORD32 d_flash_size;
    UWORD32 d_granularity;
    UWORD16 cs3_size_config;    
    UWORD16 dummy; /* For alignment Purposes */
  } T_CONF_PARAM;

#endif


  /* RSA Public key */

  typedef struct
  {
  
    UWORD32  a_Modulus[C_RSAKEYLG];  /*Public Modulus*/
    UWORD32  d_ModulusLength;        /*Public Modulus length in bytes*/
    UWORD32  d_Exponent;             /*Public Exponent*/

  }T_RSAPUBKEY;


  /*Certificate Structure*/

  /* New structure for 'certificate' for GP Device */  
  /* Note this 'certificate' structure is applicable for GP Devices only
   * for the flash programmer code i.e. during bootloading phase.
   * If bootloading times out, BootROM jumps and starts execution directly from nCS5 */
  typedef struct
  {
    UWORD32       d_Addcode;                          /* Start Address of code*/
    UWORD32       d_Codesize;                         /* Size of code*/
    UWORD32       d_CodeStartAdd;                     /* Entry point address */
    UWORD32       a_SwHash[C_FIRM_HASH_SIZE];         /* Hash of the Software*/
  } T_GPDEVICE_CERTIFICATE;


  /* Certificate structure definition for HS Device */
  typedef struct
  {

    UWORD16       d_Certsize;                         /* Size of Certificate*/
    UWORD8        d_Certtype;                         /* Type of Certificate*/
    UWORD8        d_Debugrequest;                     /* Debug request*/
    UWORD32       d_Addcode;                          /* Start Address of code*/
    UWORD32       d_Codesize;                         /* Size of code*/
    UWORD32       d_CodeStartAdd;                     /* Entry point address */
    T_RSAPUBKEY   d_Manpubkey;                        /* Manufacturer Public key*/
    T_RSAPUBKEY   d_Origpubkey;                       /* Originator Public key */
    UWORD32       a_Origpubkeysig[C_MANUF_SIG_SIZE];  /* Originator Public key Signature*/
    UWORD32       a_Swsig[C_MANUF_SIG_SIZE];          /* Software Signature*/
    T_CONF_PARAM  d_Confparam;                        /* Configuration parameters*/
    UWORD32       a_die_id[C_DIE_ID_SIZE];            /* DIE ID field */

  } T_MANUFACTURER_CERTIFICATE;


  #define C_MAN_PLATFORM_DATA_SIZE 2     /* 16048 is the maximum value allowed */

  /*
   * Manufacturer Certificate structure containing dynamic Platform Data
   */
  typedef struct {
    T_MANUFACTURER_CERTIFICATE  d_manufacturer_certificate;
    UWORD32                     a_platform_data[C_MAN_PLATFORM_DATA_SIZE];  /* Platform Data */
    UWORD32                     a_Certsig[C_MANUF_SIG_SIZE];                /* Certificate Signature*/

  } T_MANUFACTURER_CERTIFICATE_PLATFORM_DATA;

  

  #if C_SECURE_SERVICES == 1
    /*
     *  Define ROM secure service entry point address
     */
    #define C_ROM_SSERVICE_ENTRY_POINT_ADDR   0x00000024L

    /*
     *  Definition of function pointer to use ROM Secure Services
     */
    typedef UWORD16 (*T_ROM_SSERVICE) (UWORD16, void *, void *);


    typedef struct
    {

      UWORD16       d_Certsize;                   /* Size of Certificate*/
      UWORD8        d_Certtype;                   /* Type of Certificate*/
      UWORD8        d_Confrequest;                /* Confidentiality request*/
      UWORD32       d_Addcode;                    /* Start Address of code*/
      UWORD32       d_Codesize;                   /* Size of code*/
      UWORD32       d_AppID; 		                  /* Application ID*/
      UWORD32       a_iv[C_IVLG];                 /* Initial Vector for TDES CBC*/
      UWORD32       a_Encswkey[C_TDESKEYLG];      /* Encrypted S/W key */
      UWORD32       a_Swsig[C_PLAT_SIG_SIZE];     /* Software Signature*/
      UWORD32       a_Certsig[C_PLAT_SIG_SIZE];   /* Certificate Signature*/

    } T_PLATFORM_CERTIFICATE;


    /*Unbind Context Structure used for segmented Unbind operation*/

    typedef struct
    {
      UWORD32  d_Startpos;                    /*Start position of data to process */
      UWORD32  d_Currentpos;                  /*Current position of data to process */
      UWORD32  a_Currentdigest[C_MD5HASHLG];  /*Current digest*/
      UWORD32  d_Digcount;  		              /*Digest Count */
      UWORD32  a_Currentiv[C_IVLG];  		      /*Current initial vector */
      UWORD32  a_Encswkey[C_TDESKEYLG];       /*Encrypted TDES software Key */
      UWORD32  a_iv[C_IVLG];                  /*Initial Vector */
      UWORD32  d_Codesize; 		                /*Code size*/
      UWORD32  d_AppID; 		                  /*Application ID*/
      UWORD32  a_Swsig[C_PLAT_SIG_SIZE];      /*Software Signature*/
      UWORD8   d_Confrequest;                 /*Confidentiality request*/
      C_BOOLEAN  b_UnbindDone;                  /*Set to 1 if data unbinding is done*/


    } T_UNBINDCTX;


   /*Bind Context Structure used for segmented Bind operation*/

    typedef struct
    {

      UWORD32  d_Startpos;                    /*Start position of data to process*/
      UWORD32  d_Currentpos;                  /*Current position of data to process */
      UWORD32  a_Currentdigest[C_MD5HASHLG];  /*Current digest*/
      UWORD32  d_Digcount;  		              /*Digest Count */
      UWORD32  a_Currentiv[C_IVLG];  		      /*Current initial vector */
      UWORD32  a_Encswkey[C_TDESKEYLG];       /*Encrypted TDES software Key */
      UWORD32  a_iv[C_IVLG];                  /*Initial Vector */
      UWORD32  d_Addcode;  		                /*Store address of code */
      UWORD32  d_Codesize; 		                /*Code size*/
      UWORD32  d_AppID; 		                  /*Application ID*/
      UWORD8   d_Confrequest;                 /*Confidentiality request*/
      C_BOOLEAN  b_KeyCreateDone;               /*Set to 1 if TDES software key creation is done */
      C_BOOLEAN  b_BindDone;                    /*Set to 1 if data binding is done*/

    } T_BINDCTX;

 
    
    typedef struct
    {
    
       T_PLATFORM_CERTIFICATE*  p_Cert;
       UWORD32*                 p_Code;
       T_PLATFORM_CERTIFICATE*  p_CertCtx;
       T_UNBINDCTX*             p_UnbindCtx;
       UWORD32                  d_Steplength;
       C_BOOLEAN                b_start;
    
    } T_SSERVICE_UNBIND;



    typedef struct
    {

       T_PLATFORM_CERTIFICATE*   p_Cert;
       UWORD32*                  p_Code;
       T_PLATFORM_CERTIFICATE*   p_CertCtx;
       T_BINDCTX*                p_BindCtx;
       UWORD32                   d_Steplength;
       UWORD32                   d_timeout;
       C_BOOLEAN                 b_start;
    
    } T_SSERVICE_BIND;    


    /*
     *  Structure definition for RSA S/W secure services
     */
    typedef struct
    {
      UWORD32 *     p_datain;
      UWORD32 *     p_dataout;
      UWORD32       output_length;
      T_RSAPUBKEY * p_rsapubkey;
      UWORD32 *     p_rsaheap;
    } T_SSERVICE_RSA;


    /*
     *  Structure definition for Checker ROM secure services
     */
    typedef struct
    {
      UWORD16   d_checksum;
      UWORD16   d_rom_id;
    } T_SSERVICE_CHECKROM;


    /*
     *  Structure definition for Run Time Checker secure services
     */
    typedef struct
    {
      UWORD32 *                       p_rsaheap;
      T_MANUFACTURER_CERTIFICATE *    p_certificate;
    } T_SSERVICE_RUNTIMECHECKER;


    /*
     *  Structure definition for Run Time Platform Data Checker secure services
     */
    typedef struct
    {
      T_PLATFORM_CERTIFICATE*  p_Cert;
    } T_SSERVICE_RUNTIME_PLAT_CHECKER;


  #endif /*  C_SECURE_SERVICES == 1 */

  /****************************************************************************
   * Global variables
   ****************************************************************************/
  #if defined(_TMS470) || defined(__TMS470__)
    #ifdef SECURE_GLOBAL
      #pragma DATA_SECTION(d_temp,".secdata")
      #pragma DATA_SECTION(a_hash_certificate,".secdata")
    #endif
  #endif

  #ifndef SECURE_GLOBAL
    #define SECURE_GLOBAL extern
  #endif
  SECURE_GLOBAL UWORD32 d_temp[C_TEMPDATALG];               /*Temporary data in Secure RAM*/
  SECURE_GLOBAL UWORD32 a_hash_certificate[C_SHA1HASHLG];   /* SHA-1 hashing of Manufacturer Certificate */

  /****************************************************************************
   * Status 
   ****************************************************************************/

  typedef enum 
  {
  
    C_SECURE_ERROR           = 0,
    C_SECURE_SUCCESS         = 1,
    C_SECURE_INVALID_ID      = 2,
    C_SECURE_RNG_ALARM       = 3,
    C_SECURE_INVALID_DIE_ID  = 4,
    C_SECURE_BLANK_DIE_ID    = 5
  }E_SECURE_STATUS;

  typedef enum
  {
    C_GP_DEVICE = 0,
    C_HS_DEVICE = 1,
    C_DEVICE_UNKNOWN = 255
  }E_DEVICE_TYPE;


  /****************************************************************************
   * Function prototype for secure services 
   ****************************************************************************/

  #if C_SECURE_SERVICES == 1
    #define C_MAX_DEFINED_ID                                6
    #define C_SEC_SERVICE_BINDING_ID                        0x0000
    #define C_SEC_SERVICE_UNBINDING_ID                      0x0001
    #define C_SEC_SERVICE_RSA_ID                            0x0002
    #define C_SEC_SERVICE_CHECKROM_ID                       0x0003
    #define C_SEC_SERVICE_RUN_TIME_CHECKER_ID               0x0004
    #define C_SEC_SERVICE_RUN_TIME_PLATFORM_DATA_CHECKER    0x0005

    E_SECURE_STATUS ROM_Sservice_Unbind(UWORD16 reserved,
                                        T_SSERVICE_UNBIND* p_StructUnbind);

    E_SECURE_STATUS ROM_Sservice_Bind(UWORD16 reserved,
                                      T_SSERVICE_BIND* p_StructBind);

    E_SECURE_STATUS ROM_Sservice_Rsa(UWORD16           reserved,
                                     T_SSERVICE_RSA*   p_StructRsa);

    E_SECURE_STATUS ROM_Sservice_CheckRom(UWORD16               reserved,
                                          T_SSERVICE_CHECKROM*  p_StructCheckRom);

    E_SECURE_STATUS ROM_Sservice_RunTimeChecker(
                                  UWORD16                      reserved,
                                  T_SSERVICE_RUNTIMECHECKER*   p_StructRunTimeChecker);
    
    E_SECURE_STATUS ROM_Sservice_RunTimePlatformDataChecker(
                                  UWORD16                           reserved,
                                  T_SSERVICE_RUNTIME_PLAT_CHECKER*  p_StructRunTimePlatChecker);
    
  #endif

#endif /* _SECURE_TYPES_H */
