#ifndef __AT_HS_H
#define __AT_HS_H
/****************************************************************************
 *
 * File:
 *     $Workfile:at_hs.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:10$
 *
 * Description: This file contains an AT Headset Command Processor.
 *             
 * Copyright 2002-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if AT_HEADSET == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Headset Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The Headset Command Processor module provides the commands necessary
 * for the Headset Profile.  It is necessary to include this module
 * when using the Hands-free Profile, because they share some AT commands.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 *     Headset Command and Result types.
 */
#ifndef AT_COMMAND_DEFINED
#define AT_COMMAND_DEFINED
typedef U16 AtCommand;
#endif

/**  */
#define AT_MICROPHONE_GAIN      0x0100

/**  */
#define AT_SPEAKER_GAIN         0x0101

/**  */
#define AT_KEYPAD_CONTROL       0x0102

#if TI_CHANGES == XA_ENABLED

/*
 * Hands-free audio gateway sends AT+VGM: <n>
 * <n> stands for the microphone gain.
 */
#define AT_HF_MICROPHONE_GAIN	0x0103

/*
 * Hands-free audio gateway sends AT+VGS: <n>
 * <n> stands for the speaker gain.
 */
#define AT_HF_SPEAKER_GAIN		0x0104

/*
 * Headset audio gateway sends AT+VGM=<n>
 * <n> stands for the microphone gain.
 */
#define AT_HS_MICROPHONE_GAIN	0x0105

/*
 * Headset audio gateway sends AT+VGS=<n>
 * <n> stands for the speaker gain.
 */
#define AT_HS_SPEAKER_GAIN		0x0106

#endif /* TI_CHANGES == XA_ENABLED */

/* End of AtCommand */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtHeadsetCmd
 * 
 *     This structure is used to specify the parameters associated with
 *     Headset commands. Headset commands are sent from the Headset device
 *     to the Audio Gateway. As such, this structure is used by the Headset
 *     device encoder, and the Audio Gateway decoder functions.
 */
typedef union _AtHeadsetCmd {

    /* AT_MICROPHONE_GAIN */
    struct {
        U8          gain;
    } mic;

    /* AT_SPEAKER_GAIN */
    struct {
        U8          gain;
    } speaker;

    /* AT_KEYPAD_CONTROL */
    struct {
        U8          button;
    } keypad;

} AtHeadsetCmd;
    

/*---------------------------------------------------------------------------
 * AtHeadsetResult
 * 
 *     This structure is used to specify the parameters associated with
 *     Headset results. Headset results are sent from the Audio Gateway
 *     to the Headset device. As such, this structure is used by the Audio
 *     Gateway encoder, and the Headset device decoder functions.
 */
typedef union _AtHeadsetResult {

    /* AT_MICROPHONE_GAIN */
    struct {
        U8          gain;
    } mic;  

    /* AT_SPEAKER_GAIN */
    struct {
        U8          gain;
    } speaker;  

} AtHeadsetResult;

#else /* AT_HEADSET == XA_ENABLED */

/* Stub structures to keep #defines out of AtResults/AtCommands structures. */
typedef U8 AtHeadsetCmd;
typedef U8 AtHeadsetResult;

#endif /* AT_HEADSET == XA_ENABLED */

#endif /* __AT_HS_H */
