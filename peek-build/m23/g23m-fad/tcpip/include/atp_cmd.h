/*******************************************************************************
 *
 * File Name : atp_cmd.h
 *
 * External definition for commands supported by ATP
 *
 * (C) Texas Instruments, all rights reserved
 *
 * Version number  : 0.1      Date : 7-Marsh-2000
 *
 * History      : 0.1  - Created by E. Baissus
 *
 *
 * Author           : Eric Baissus : e-baissus@ti.com
 *
 *   (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved
 ******************************************************************************/
#include "rv_general.h"
#include "rvf_api.h"
#include "atp_config.h"
#include "atp_api.h"

#include <stdlib.h>

#ifndef ATP_CMD_H
#define ATP_CMD_H

/************************** LOCAL DCE-DTE PORT CONTROL **************************/
/*                                                                              */
/* Define the number of word needed to mask the commands executed by the ATP.   */
#define NB_WORD_OF_DCE_CMD_MASK (0x01)

/* Define the default command line termination character (See ITU-T             */
/* Recommendation V.250 ter page 21).                                           */
#define ATP_CR_CHARACTER ('\x0D')

/* Define the default response formatting character (See ITU-T Recommendation   */
/* V.250 ter page 22).                                                          */
#define ATP_LF_CHARACTER ('\x0A')

/* Define the default command line editing character (See ITU-T Recommendation  */
/* V.250 ter page 22).                                                          */
#define ATP_BS_CHARACTER ('\x08')

/* Define the escape sequence (See ITU-T Recommendation V.250 ter page 24).     */
#define MAX_NB_OF_CHARACTER_FOR_END_SEQUENCE (0x03)

static const char ATP_ESCAPE_SEQUENCE[] = "+++";

/* Define the command echo (See ITU-T Recommendation V.250 ter page 23).        */
typedef enum
{
  ECHO_OFF = 0x00000000,
  ECHO_ON
} T_ATP_ECHO_MODE;

/* Define the result code suppression (See ITU-T Recommendation V.250 ter page  */
/* 23).                                                                         */
typedef enum
{
  RESULT_CODE_ON = 0x00000000,
  RESULT_CODE_OFF
} T_ATP_RESULT_CODE_MODE;

/* Define the DCE response format (See ITU-T Recommendation V.250 ter page 24). */
typedef enum
{
  ATP_VERBOSE_0 = 0x00000000,
  ATP_VERBOSE_1
} T_ATP_VERBOSE_MODE;

/* Define the structure used to store information related to DCE handling.      */
typedef UINT16 T_ATP_DCE_MASK[NB_WORD_OF_DCE_CMD_MASK];

typedef struct
{
  char cr_character;
  char lf_character;
  char bs_character;
  char escape_sequence[MAX_NB_OF_CHARACTER_FOR_END_SEQUENCE];
  T_ATP_ECHO_MODE echo_mode;
  T_ATP_RESULT_CODE_MODE result_code_mode;
  T_ATP_VERBOSE_MODE verbose_mode;
  T_ATP_DCE_MASK dce_mask;
  UINT8 nb_plus_received; // Number of exit character that have been previously received
  UINT8 *escape_sequence_tmp_buffer_p[MAX_NB_OF_CHARACTER_FOR_END_SEQUENCE]; // Used as temp buffer by the escape sequence algo
  UINT8 length_of_escape_sequence_tmp_buffer_p[MAX_NB_OF_CHARACTER_FOR_END_SEQUENCE];
} T_ATP_DCE_INFO;

/********************************** AT COMMANDS *********************************/
/*                                                                              */
/* Define all supported AT commands.                                            */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |    ATP_ATA_NB     |  1  |   ATA   | Answer.                              | */
/* |    ATP_ATD_NB     |  1  |   ATD   | Dial.                                | */
/* |    ATP_ATE_NB     |  1  |   ATE   | Command echo.                        | */
/* |    ATP_ATH_NB     |  1  |   ATH   | Hook control.                        | */
/* |    ATP_ATI_NB     |  1  |   ATI   | Request Identification Information   | */
/* |    ATP_ATL_NB     |  1  |   ATL   | Monitor speaker loudness.            | */
/* |    ATP_ATM_NB     |  1  |   ATM   | Monitor speaker mode.                | */
/* |    ATP_ATO_NB     |  1  |   ATO   | Return to online data state.         | */
/* |    ATP_ATP_NB     |  1  |   ATP   | Select pulse dialling.               | */
/* |    ATP_ATQ_NB     |  1  |   ATQ   | Result code suppression.             | */
/* |    ATP_ATS0_NB    |  2  |   ATS0  | Automatic answer.                    | */
/* |    ATP_ATS10_NB   |  3  |  ATS10  | Automatic disconnect delay.          | */
/* |    ATP_ATS3_NB    |  2  |   ATS3  | Command line termination character.  | */
/* |    ATP_ATS4_NB    |  2  |   ATS4  | Response formatting character.       | */
/* |    ATP_ATS5_NB    |  2  |   ATS5  | Command line editing character.      | */
/* |    ATP_ATS6_NB    |  2  |   ATS6  | Pause before blind dialling.         | */
/* |    ATP_ATS7_NB    |  2  |   ATS7  | Connection completion timeout.       | */
/* |    ATP_ATS8_NB    |  2  |   ATS8  | Comma dial modifier time.            | */
/* |    ATP_ATT_NB     |  1  |   ATT   | Select tone dialling.                | */
/* |    ATP_ATV_NB     |  1  |   ATV   | DCE response format.                 | */
/* |    ATP_ATX_NB     |  1  |   ATX   | Result code selection and call       | */
/* |                   |     |         | progress monitoring control.         | */
/* |    ATP_ATZ_NB     |  1  |   ATZ   | Reset to default configuration.      | */
/* |  ATP_AT_AND_C_NB  |  2  |   AT&C  | Circuit 109 (received line signal    | */
/* |                   |     |         | detector) behavior.                  | */
/* |  ATP_AT_AND_D_NB  |  2  |   AT&D  | Circuit 108 (data terminal ready)    | */
/* |                   |     |         | behavior.                            | */
/* |  ATP_AT_AND_F_NB  |  2  |   AT&F  | Set to factory defined               | */
/* |                   |     |         | configuration.                       | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |ATP_AT_PLUS_CKPD_NB|  5  | AT+CKPD | Keypad control command (See GSM TS   | */
/* |                   |     |         | 07.07, Section 8.7). For <keys>, the | */
/* |                   |     |         | value of 200 indicates the button on | */
/* |                   |     |         | the headset being pushed. The <time> | */
/* |                   |     |         | and <pause> parameters have no       | */
/* |                   |     |         | meaning in the headset profile.      | */
/* |ATP_AT_PLUS_FDR_NB |  4  | AT+FDR  | Receive a page.                      | */
/* |ATP_AT_PLUS_FDT_NB |  4  | AT+FDT  | Send a page.                         | */
/* |ATP_AT_PLUS_FKS_NB |  4  | AT+FKS  | Session termination.                 | */
/* |ATP_AT_PLUS_GMI_NB |  4  | AT+GMI  | Request manufacturer identification. | */
/* |ATP_AT_PLUS_GMM_NB |  4  | AT+GMM  | Request model identification.        | */
/* |ATP_AT_PLUS_GMR_NB |  4  | AT+GMR  | Request revision identification.     | */
/* |ATP_AT_PLUS_GCAP_NB|  5  | AT+GCAP | Request complete capabilities list.  | */
/* |ATP_AT_PLUS_VGM_NB |  4  | AT+VGM  | Command issued by the HS to report   | */
/* |                   |     |         | the current microphone gain level    | */
/* |                   |     |         | setting to the AG (See Headset       | */
/* |                   |     |         | Specification, Section 4.7.3).       | */
/* |ATP_AT_PLUS_VGS_NB |  4  | AT+VGS  | Command issued by the HS to report   | */
/* |                   |     |         | the current speaker gain level       | */
/* |                   |     |         | setting to the AG (See Headset       | */
/* |                   |     |         | Specification, Section 4.7.3).       | */
/* |___________________|_____|_________|______________________________________| */
/*                                                                              */
/* Define the command line prefix "AT" (See ITU-T Recommendation V.250 ter page */
/* page 4).                                                                     */
#define ATP_AT_PREFIX ("AT")
#define ATP_AT_PREFIX_LEN (0x02)

/* Text codes for AT commands.                                                  */
static const char ATP_AT_TXT_TABLE[] = {'A', \
                    'D', \
                    'E', \
                    'H', \
                    'I', \
                    'L', \
                    'M', \
                    'O', \
                    'P', \
                    'Q', \
                    'S', '0', \
                    'S', '1', '0', \
                    'S', '3', \
                    'S', '4', \
                    'S', '5', \
                    'S', '6', \
                    'S', '7', \
                    'S', '8', \
                    'T', \
                    'V', \
                    'X', \
                    'Z', \
                    '&', 'C', \
                    '&', 'D', \
                    '&', 'F', \
                    '+', 'C', 'K', 'P', 'D', \
                    '+', 'F', 'D', 'R', \
                    '+', 'F', 'D', 'T', \
                    '+', 'F', 'K', 'S', \
                    '+', 'G', 'M', 'I', \
                    '+', 'G', 'M', 'M', \
                    '+', 'G', 'M', 'R', \
                    '+', 'G', 'C', 'A', 'P', \
                    '+', 'V', 'G', 'M', \
                    '+', 'V', 'G', 'S', \
                    '\xFF'};

/* Binary related codes.                                                        */
typedef enum
{
  ATP_ATA_NB = (0x00000000),
  ATP_ATD_NB,
  ATP_ATE_NB,
  ATP_ATH_NB,
  ATP_ATI_NB,
  ATP_ATL_NB,
  ATP_ATM_NB,
  ATP_ATO_NB,
  ATP_ATP_NB,
  ATP_ATQ_NB,
  ATP_ATS0_NB,
  ATP_ATS10_NB,
  ATP_ATS3_NB,
  ATP_ATS4_NB,
  ATP_ATS5_NB,
  ATP_ATS6_NB,
  ATP_ATS7_NB,
  ATP_ATS8_NB,
  ATP_ATT_NB,
  ATP_ATV_NB,
  ATP_ATX_NB,
  ATP_ATZ_NB,
  ATP_AT_AND_C_NB,
  ATP_AT_AND_D_NB,
  ATP_AT_AND_F_NB,
  ATP_AT_PLUS_CKPD_NB,
  ATP_AT_PLUS_FDR_NB,
  ATP_AT_PLUS_FDT_NB,
  ATP_AT_PLUS_FKS_NB,
  ATP_AT_PLUS_GMI_NB,
  ATP_AT_PLUS_GMM_NB,
  ATP_AT_PLUS_GMR_NB,
  ATP_AT_PLUS_GCAP_NB,
  ATP_AT_PLUS_VGM_NB,
  ATP_AT_PLUS_VGS_NB,
  ATP_MAX_NB_OF_AT_COMMANDS
} T_ATP_BINARY_AT_COMMANDS;

/* Related offsets.                                                             */
#define ATP_ATA_OFFSET (0x00)
#define ATP_ATD_OFFSET (ATP_ATA_OFFSET + 0x01)
#define ATP_ATE_OFFSET (ATP_ATD_OFFSET + 0x01)
#define ATP_ATH_OFFSET (ATP_ATE_OFFSET + 0x01)
#define ATP_ATI_OFFSET (ATP_ATH_OFFSET + 0x01)
#define ATP_ATL_OFFSET (ATP_ATI_OFFSET + 0x01)
#define ATP_ATM_OFFSET (ATP_ATL_OFFSET + 0x01)
#define ATP_ATO_OFFSET (ATP_ATM_OFFSET + 0x01)
#define ATP_ATP_OFFSET (ATP_ATO_OFFSET + 0x01)
#define ATP_ATQ_OFFSET (ATP_ATP_OFFSET + 0x01)
#define ATP_ATS0_OFFSET (ATP_ATQ_OFFSET + 0x01)
#define ATP_ATS10_OFFSET (ATP_ATS0_OFFSET + 0x02)
#define ATP_ATS3_OFFSET (ATP_ATS10_OFFSET + 0x03)
#define ATP_ATS4_OFFSET (ATP_ATS3_OFFSET + 0x02)
#define ATP_ATS5_OFFSET (ATP_ATS4_OFFSET + 0x02)
#define ATP_ATS6_OFFSET (ATP_ATS5_OFFSET + 0x02)
#define ATP_ATS7_OFFSET (ATP_ATS6_OFFSET + 0x02)
#define ATP_ATS8_OFFSET (ATP_ATS7_OFFSET + 0x02)
#define ATP_ATT_OFFSET (ATP_ATS8_OFFSET + 0x02)
#define ATP_ATV_OFFSET (ATP_ATT_OFFSET + 0x01)
#define ATP_ATX_OFFSET (ATP_ATV_OFFSET + 0x01)
#define ATP_ATZ_OFFSET (ATP_ATX_OFFSET + 0x01)
#define ATP_AT_AND_C_OFFSET (ATP_ATZ_OFFSET + 0x01)
#define ATP_AT_AND_D_OFFSET (ATP_AT_AND_C_OFFSET + 0x02)
#define ATP_AT_AND_F_OFFSET (ATP_AT_AND_D_OFFSET + 0x02)
#define ATP_AT_PLUS_CKPD_OFFSET (ATP_AT_AND_F_OFFSET + 0x02)
#define ATP_AT_PLUS_FDR_OFFSET (ATP_AT_PLUS_CKPD_OFFSET + 0x05)
#define ATP_AT_PLUS_FDT_OFFSET (ATP_AT_PLUS_FDR_OFFSET + 0x04)
#define ATP_AT_PLUS_FKS_OFFSET (ATP_AT_PLUS_FDT_OFFSET + 0x04)
#define ATP_AT_PLUS_GMI_OFFSET (ATP_AT_PLUS_FKS_OFFSET + 0x04)
#define ATP_AT_PLUS_GMM_OFFSET (ATP_AT_PLUS_GMI_OFFSET + 0x04)
#define ATP_AT_PLUS_GMR_OFFSET (ATP_AT_PLUS_GMM_OFFSET + 0x04)
#define ATP_AT_PLUS_GCAP_OFFSET (ATP_AT_PLUS_GMR_OFFSET + 0x04)
#define ATP_AT_PLUS_VGM_OFFSET (ATP_AT_PLUS_GCAP_OFFSET + 0x05)
#define ATP_AT_PLUS_VGS_OFFSET (ATP_AT_PLUS_VGM_OFFSET + 0x04)
#define ATP_AT_LAST_OFFSET (ATP_AT_PLUS_VGS_OFFSET + 0x04)

/* Related structures.                                                          */
typedef enum
{
  ATP_CMD_NOT_DEFINED = (0x00000000),
  ATP_NO_PARAM,
  ATP_BASIC_PARAM,
  ATP_DIAL_PARAM,
  ATP_S_PARAM,
  ATP_NO_EXTENDED_PARAM,
  ATP_SINGLE_EXTENDED_PARAM,
  ATP_PLUS_CKPD_PARAM
} T_ATP_CMD_FORMAT;

#define ATP_ATA_PARAM (ATP_NO_PARAM)
#define ATP_ATD_PARAM (ATP_DIAL_PARAM)
#define ATP_ATE_PARAM (ATP_BASIC_PARAM)
#define ATP_ATH_PARAM (ATP_BASIC_PARAM)
#define ATP_ATI_PARAM (ATP_BASIC_PARAM)
#define ATP_ATL_PARAM (ATP_BASIC_PARAM)
#define ATP_ATM_PARAM (ATP_BASIC_PARAM)
#define ATP_ATO_PARAM (ATP_BASIC_PARAM)
#define ATP_ATP_PARAM (ATP_NO_PARAM)
#define ATP_ATQ_PARAM (ATP_BASIC_PARAM)
#define ATP_ATS0_PARAM (ATP_S_PARAM)
#define ATP_ATS10_PARAM (ATP_S_PARAM)
#define ATP_ATS3_PARAM (ATP_S_PARAM)
#define ATP_ATS4_PARAM (ATP_S_PARAM)
#define ATP_ATS5_PARAM (ATP_S_PARAM)
#define ATP_ATS6_PARAM (ATP_S_PARAM)
#define ATP_ATS7_PARAM (ATP_S_PARAM)
#define ATP_ATS8_PARAM (ATP_S_PARAM)
#define ATP_ATT_PARAM (ATP_NO_PARAM)
#define ATP_ATV_PARAM (ATP_BASIC_PARAM)
#define ATP_ATX_PARAM (ATP_BASIC_PARAM)
#define ATP_ATZ_PARAM (ATP_BASIC_PARAM)
#define ATP_AT_AND_C_PARAM (ATP_BASIC_PARAM)
#define ATP_AT_AND_D_PARAM (ATP_BASIC_PARAM)
#define ATP_AT_AND_F_PARAM (ATP_BASIC_PARAM)
#define ATP_AT_PLUS_CKPD_PARAM (ATP_PLUS_CKPD_PARAM)
#define ATP_AT_PLUS_FDR_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_FDT_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_FKS_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_GMI_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_GMM_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_GMR_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_GCAP_PARAM (ATP_NO_EXTENDED_PARAM)
#define ATP_AT_PLUS_VGM_PARAM (ATP_SINGLE_EXTENDED_PARAM)
#define ATP_AT_PLUS_VGS_PARAM (ATP_SINGLE_EXTENDED_PARAM)

#define ATP_AT_OFFSET_COLUMN (0x00)
#define ATP_AT_PARAM_COLUMN (0x01)

static const UINT8 ATP_AT_INFO[ATP_MAX_NB_OF_AT_COMMANDS + 0x01][0x02] \
           = {{ATP_ATA_OFFSET, ATP_ATA_PARAM}, \
            {ATP_ATD_OFFSET, ATP_ATD_PARAM}, \
            {ATP_ATE_OFFSET, ATP_ATE_PARAM}, \
            {ATP_ATH_OFFSET, ATP_ATH_PARAM}, \
            {ATP_ATI_OFFSET, ATP_ATI_PARAM}, \
            {ATP_ATL_OFFSET, ATP_ATL_PARAM}, \
            {ATP_ATM_OFFSET, ATP_ATM_PARAM}, \
            {ATP_ATO_OFFSET, ATP_ATO_PARAM}, \
            {ATP_ATP_OFFSET, ATP_ATP_PARAM}, \
            {ATP_ATQ_OFFSET, ATP_ATQ_PARAM}, \
            {ATP_ATS0_OFFSET, ATP_ATS0_PARAM}, \
            {ATP_ATS10_OFFSET, ATP_ATS10_PARAM}, \
            {ATP_ATS3_OFFSET, ATP_ATS3_PARAM}, \
            {ATP_ATS4_OFFSET, ATP_ATS4_PARAM}, \
            {ATP_ATS5_OFFSET, ATP_ATS5_PARAM}, \
            {ATP_ATS6_OFFSET, ATP_ATS6_PARAM}, \
            {ATP_ATS7_OFFSET, ATP_ATS7_PARAM}, \
            {ATP_ATS8_OFFSET, ATP_ATS8_PARAM}, \
            {ATP_ATT_OFFSET, ATP_ATT_PARAM}, \
            {ATP_ATV_OFFSET, ATP_ATV_PARAM}, \
            {ATP_ATX_OFFSET, ATP_ATX_PARAM}, \
            {ATP_ATZ_OFFSET, ATP_ATZ_PARAM}, \
            {ATP_AT_AND_C_OFFSET, ATP_AT_AND_C_PARAM}, \
            {ATP_AT_AND_D_OFFSET, ATP_AT_AND_D_PARAM}, \
            {ATP_AT_AND_F_OFFSET, ATP_AT_AND_F_PARAM}, \
            {ATP_AT_PLUS_CKPD_OFFSET, ATP_AT_PLUS_CKPD_PARAM}, \
            {ATP_AT_PLUS_FDR_OFFSET, ATP_AT_PLUS_FDR_PARAM}, \
            {ATP_AT_PLUS_FDT_OFFSET, ATP_AT_PLUS_FDT_PARAM}, \
            {ATP_AT_PLUS_FKS_OFFSET, ATP_AT_PLUS_FKS_PARAM}, \
            {ATP_AT_PLUS_GMI_OFFSET, ATP_AT_PLUS_GMI_PARAM}, \
            {ATP_AT_PLUS_GMM_OFFSET, ATP_AT_PLUS_GMM_PARAM}, \
            {ATP_AT_PLUS_GMR_OFFSET, ATP_AT_PLUS_GMR_PARAM}, \
            {ATP_AT_PLUS_GCAP_OFFSET, ATP_AT_PLUS_GCAP_PARAM}, \
            {ATP_AT_PLUS_VGM_OFFSET, ATP_AT_PLUS_VGM_PARAM}, \
            {ATP_AT_PLUS_VGS_OFFSET, ATP_AT_PLUS_VGS_PARAM}, \
            {ATP_AT_LAST_OFFSET, ATP_CMD_NOT_DEFINED}};

/* Define the maximum length (text format) of <numbers> appearing in basic      */
/* syntax commands.                                                             */
/*    - <number> (up to 65535).                                                 */
#define MAX_BASIC_CMD_PARAM_LEN (0x05)

/* Define the structure associated with basic syntax commands (except for the   */
/* D and S commands). The format of such commands is <command>[<number>], where */
/* <number> may be a string of one or more characters from "0" through "9"      */
/* representing the decimal integer value (See ITU-T Recommendation V.250 ter   */
/* page 5).                                                                     */
typedef struct
{
  UINT16 number;
} T_ATP_BASIC_CMD;

/* Define all basic syntax commands do not expext any <number>.                 */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |    ATP_ATA_NB     |  1  |   ATA   | Answer.                              | */
/* |    ATP_ATP_NB     |  1  |   ATP   | Select pulse dialling.               | */
/* |    ATP_ATT_NB     |  1  |   ATT   | Select tone dialling.                | */
/* |___________________|_____|_________|______________________________________| */
/*                                                                              */
/* Define all other basic syntax commands.                                      */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |    ATP_ATE_NB     |  1  |   ATE   | Command echo.                        | */
/* |    ATP_ATH_NB     |  1  |   ATH   | Hook control.                        | */
/* |    ATP_ATI_NB     |  1  |   ATI   | Request Identification Information   | */
/* |    ATP_ATL_NB     |  1  |   ATL   | Monitor speaker loudness.            | */
/* |    ATP_ATM_NB     |  1  |   ATM   | Monitor speaker mode.                | */
/* |    ATP_ATO_NB     |  1  |   ATO   | Return to online data state.         | */
/* |    ATP_ATQ_NB     |  1  |   ATQ   | Result code suppression.             | */
/* |    ATP_ATV_NB     |  1  |   ATV   | DCE response format.                 | */
/* |    ATP_ATX_NB     |  1  |   ATX   | Result code selection and call       | */
/* |                   |     |         | progress monitoring control.         | */
/* |    ATP_ATZ_NB     |  1  |   ATZ   | Reset to default configuration.      | */
/* |  ATP_AT_AND_C_NB  |  2  |   AT&C  | Circuit 109 (received line signal    | */
/* |                   |     |         | detector) behavior.                  | */
/* |  ATP_AT_AND_D_NB  |  2  |   AT&D  | Circuit 108 (data terminal ready)    | */
/* |                   |     |         | behavior.                            | */
/* |  ATP_AT_AND_F_NB  |  2  |   AT&F  | Set to factory defined               | */
/* |                   |     |         | configuration.                       | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_BASIC_CMD T_ATP_ATE_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATH_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATI_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATL_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATM_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATO_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATQ_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATV_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATX_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_ATZ_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_AT_AND_C_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_AT_AND_D_PARAM;
typedef T_ATP_BASIC_CMD T_ATP_AT_AND_F_PARAM;

/* Define the maximum number of characters appearing in the <dial_string>.      */
#define MAX_DIAL_STRING_LEN (30)

/* Define the structure associated with "Dial" command whose format is          */
/* D[<dial_string>][;] (See ITU-T Recommendation V.250 ter page 31). Note that  */
/* all characters appearing on the same command line after D are considered     */
/* part of the call addressing information to be signalled to the network, or   */
/* modifiers used to control the signalling process, up to a semicolon          */
/* character or the end of the command line. If the <dial_string> is terminated */
/* by a semicolon, the DCE does not start the call origination procedure, but   */
/* instead returns to command state after completion of the signalling of call  */
/* addressing information to the network.                                       */
typedef struct
{
  char dial_string_p[MAX_DIAL_STRING_LEN];
  UINT8 dial_string_length;
  enum
  {
    DATA_CALL = 0x00000000,
    VOICE_CALL
  } call_type;
} T_ATP_DIAL;

/* Define all dial commands.                                                    */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |    ATP_ATD_NB     |  1  |   ATD   | Dial.                                | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_DIAL T_ATP_ATD_PARAM;

/* Define the maximum length (text format) of <values> appearing in             */
/* S-parameters.                                                                */
/*    - '=' + <value> (up to 65535).                                            */
#define MAX_S_PARAM_LEN (0x06)

/* Define the structure associated with S-parameters. Formats of such commands  */
/* are S<parameter_number>? and S<parameter_number>=[<value>], where <value>    */
/* shall consist of a decimal constant  (See ITU-T Recommendation V.250 ter     */
/* pages 5 and 6).                                                              */
typedef struct
{
  enum
  {
    READ_S_PARAM = 0x00000001,
    SET_S_PARAM
  } s_operator;
  UINT16 value;
} T_ATP_S_PARAM;

/* Define all S-parameters.                                                     */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |    ATP_ATS0_NB    |  2  |   ATS0  | Automatic answer.                    | */
/* |    ATP_ATS10_NB   |  3  |  ATS10  | Automatic disconnect delay.          | */
/* |    ATP_ATS3_NB    |  2  |   ATS3  | Command line termination character.  | */
/* |    ATP_ATS4_NB    |  2  |   ATS4  | Response formatting character.       | */
/* |    ATP_ATS5_NB    |  2  |   ATS5  | Command line editing character.      | */
/* |    ATP_ATS6_NB    |  2  |   ATS6  | Pause before blind dialling.         | */
/* |    ATP_ATS7_NB    |  2  |   ATS7  | Connection completion timeout.       | */
/* |    ATP_ATS8_NB    |  2  |   ATS8  | Comma dial modifier time.            | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_S_PARAM T_ATP_ATS0_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS10_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS3_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS4_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS5_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS6_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS7_PARAM;
typedef T_ATP_S_PARAM T_ATP_ATS8_PARAM;

/* Define operators dedicated to extended syntax commands. "=" is used to set   */
/* the indicated extended syntax command to a new value (See ITU-T              */
/* Recommendation V.250 ter page 8). Note that if no value is given, the        */
/* extended syntax command specified may be set to 0. "?" is used to read the   */
/* current value of the indicated extended syntax command (See ITU-T            */
/* Recommendation V.250 ter page 9). "=?" is used to test whether the extended  */
/* syntax command is implemented in the DCE.                                    */
typedef enum
{
  NO_SUBPARAMETER = 0x00000000,
  READ_EXTENDED_CMD,
  SET_EXTENDED_CMD,
  TEST_EXTENDED_CMD
} T_ATP_EXTENDED_OPERATOR;

/* Define the maximum length (text format) of extended syntax commands that     */
/* have no subparameter.                                                        */
/*    - '=?'.                                                                   */
#define MAX_NO_SUBPARAMETER_LEN (0x02)

/* Define the structure associated with extended syntax commands that have no   */
/* subparameter. Formats of such commands are +<name> and +<name>=?, where      */
/* <name> may be a string of one or sixteen characters selected from the        */
/* following set (See ITU-T Recommendation V.250 ter page 6):                   */
/*   "A" through "Z",                                                           */
/*   "0" through "9",                                                           */
/*   "!", "%", "-", ".", "/", ":" and "_".                                      */
typedef struct
{
  T_ATP_EXTENDED_OPERATOR extended_operator;
} T_ATP_NO_SUBPARAMETER;

/* Define all extended syntax commands do not expext any <value>.               */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |ATP_AT_PLUS_FDR_NB |  4  | AT+FDR  | Receive a page.                      | */
/* |ATP_AT_PLUS_FDT_NB |  4  | AT+FDT  | Send a page.                         | */
/* |ATP_AT_PLUS_FKS_NB |  4  | AT+FKS  | Session termination.                 | */
/* |ATP_AT_PLUS_GMI_NB |  4  | AT+GMI  | Request manufacturer identification. | */
/* |ATP_AT_PLUS_GMM_NB |  4  | AT+GMM  | Request model identification.        | */
/* |ATP_AT_PLUS_GMR_NB |  4  | AT+GMR  | Request revision identification.     | */
/* |ATP_AT_PLUS_GCAP_NB|  5  | AT+GCAP | Request complete capabilities list.  | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_FDR_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_FDT_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_FKS_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_GMI_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_GMM_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_GMR_PARAM;
typedef T_ATP_NO_SUBPARAMETER T_ATP_AT_PLUS_GCAP_PARAM;

/* Define the maximum length (text format) of <values> appearing in extended    */
/* syntax commands whose subparameter is a numeric constant.                    */
/*    - '=' + <value> (up to 65535).                                            */
#define MAX_SINGLE_SUBPARAMETER_LEN (0x06)

/* Define the structure associated with extended syntax commands whose          */
/* subparameter is a numeric constant. Formats of such commands are +<name>?,   */
/* +<name>[=<value>] and and +<name>=?, where <name> may be a string of one or  */
/* sixteen characters selected from the following set (See ITU-T Recommendation */
/* V.250 ter page 6):                                                           */
/*   "A" through "Z",                                                           */
/*   "0" through "9",                                                           */
/*   "!", "%", "-", ".", "/", ":" and "_".                                      */
/*                                                                              */
/* <value> shall consist of a numeric constant (See ITU-T Recommendation V.250  */
/* ter page 6).                                                                 */
/* Some additional commands may follow an extended syntax command on the same   */
/* command line if a semicolon is inserted after the preceding extended command */
/* as a separator. The semicolon is not necessary when the extended syntax      */
/* command is the last command on the command line (See ITU-T Recommendation    */
/* V.250 ter page 9). Besides, extended syntax commands may appear on the same  */
/* command line after a basic syntax command without a separator, in the same   */
/* manner as concatenation of basic syntax commands.                            */
typedef struct
{
  T_ATP_EXTENDED_OPERATOR extended_operator;
  UINT16 value;
} T_ATP_SINGLE_SUBPARAMETER;

/* Define all extended syntax commands whose subparameter is a numeric          */
/* constant.                                                                    */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         | meaning in the headset profile.      | */
/* |ATP_AT_PLUS_VGM_NB |  4  | AT+VGM  | Command issued by the HS to report   | */
/* |                   |     |         | the current microphone gain level    | */
/* |                   |     |         | setting to the AG (See Headset       | */
/* |                   |     |         | Specification, Section 4.7.3).       | */
/* |ATP_AT_PLUS_VGS_NB |  4  | AT+VGS  | Command issued by the HS to report   | */
/* |                   |     |         | the current speaker gain level       | */
/* |                   |     |         | setting to the AG (See Headset       | */
/* |                   |     |         | Specification, Section 4.7.3).       | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_SINGLE_SUBPARAMETER T_ATP_AT_PLUS_VGM_PARAM;
typedef T_ATP_SINGLE_SUBPARAMETER T_ATP_AT_PLUS_VGS_PARAM;

/* Define the maximum number of keys appearing in the <keys>.                   */
#define MAX_NB_OF_KEYS (5)

/* Define the maximum length (text format) of the <compound_value> appearing in */
/* keypad control commands.                                                     */
/*    - "=" + """ + MAX_NB_OF_KEYS * [";" + <keys> (up to 65535) + ";"] + """ + */
/*          + "," + <time> (up to 65535)                                        */
/*          + "," + <pause> (up to 65535).                                      */
#define MAX_CKPD_PARAM_LEN (0x0F + MAX_NB_OF_KEYS * 0x07)

/* Define the structure associated with the extended syntax command +CKPD.      */
/* Formats of such command are +<name>=<keys>[,<time>[,<pause>]] and +<name>=?, */
/* where <name> may be a string of one or sixteen characters selected from the  */
/* following set (See ITU-T Recommendation V.250 ter page 6):                   */
/*   "A" through "Z",                                                           */
/*   "0" through "9",                                                           */
/*   "!", "%", "-", ".", "/", ":" and "_".                                      */
/*                                                                              */
/* <keys> shall consist of a string constant (See ETS 300 916 (GSM 07.07)       */
/* Version 5.8.1 page 62).                                                      */
/* <time> and <pause> shall consist of numeric constants in tenths of a second  */
/* (See ETS 300 916 (GSM 07.07) Version 5.8.1 page 62).                         */
/* Some additional commands may follow an extended syntax command on the same   */
/* command line if a semicolon is inserted after the preceding extended command */
/* as a separator. The semicolon is not necessary when the extended syntax      */
/* command is the last command on the command line (See ITU-T Recommendation    */
/* V.250 ter page 9). Besides, extended syntax commands may appear on the same  */
/* command line after a basic syntax command without a separator, in the same   */
/* manner as concatenation of basic syntax commands.                            */
typedef struct
{
  T_ATP_EXTENDED_OPERATOR extended_operator;
  UINT16 keys[MAX_NB_OF_KEYS];
  UINT8 nb_keys;
  UINT16 time;
  UINT16 pause;
} T_ATP_PLUS_CKPD;

/* Define all extended syntax commands.                                         */
/*  __________________________________________________________________________  */
/* |                   |     |         |                                      | */
/* |    Binary code    | Len | Command |              Description             | */
/* |___________________|_____|_________|______________________________________| */
/* |                   |     |         |                                      | */
/* |ATP_AT_PLUS_CKPD_NB|  5  | AT+CKPD | Keypad control command (See GSM TS   | */
/* |                   |     |         | 07.07, Section 8.7). For <keys>, the | */
/* |                   |     |         | value of 200 indicates the button on | */
/* |                   |     |         | the headset being pushed. The <time> | */
/* |                   |     |         | and <pause> parameters have no       | */
/* |                   |     |         | meaning in the headset profile.      | */
/* |___________________|_____|_________|______________________________________| */
typedef T_ATP_PLUS_CKPD T_ATP_AT_PLUS_CKPD_PARAM;

/* Define the default time value.                                               */
#define DEFAULT_TIME (0x0000)

/* Define an unspecified time value.                                            */
#define TIME_DO_NOT_CARE (0x0000)

/* Define the default pause value.                                              */
#define DEFAULT_PAUSE (0x0000)

/* Define an unspecified pause value.                                           */
#define PAUSE_DO_NOT_CARE (0x0000)

/********************************* RESULT CODES *********************************/
/*                                                                              */
/* Define all supported result codes.                                           */
/*                                                                              */
/*  __________________________________________________________________________  */
/* |                  |     |             |         |                         | */
/* |    Binary code   | Len | Result code | Numeric |       Description       | */
/* |__________________|_____|_____________|_________|_________________________| */
/* |                  |     |             |         |                         | */
/* |    ATP_OK_NB     |  2  |     OK      |    0    | Acknoledges execution   | */
/* |                  |     |             |         | of a command.           | */
/* | ATP_CONNECT_NB   |  7  |   CONNECT   |    1    | A connection has been   | */
/* |                  |     |             |         | established.            | */
/* |   ATP_RING_NB    |  4  |    RING     |    2    | Incoming call detected. | */
/* |ATP_NO_CARRIER_NB |  10 |  NO CARRIER |    3    | The connection has been | */
/* |                  |     |             |         | terminated or the       | */
/* |                  |     |             |         | attempt to establish a  | */
/* |                  |     |             |         | connection failed.      | */
/* |   ATP_ERROR_NB   |  5  |    ERROR    |    4    | Problem with processing | */
/* |                  |     |             |         | the command line.       | */
/* |ATP_NO_DIALTONE_NB|  11 | NO DIALTONE |    6    | No dial tone detected.  | */
/* |   ATP_BUSY_NB    |  4  |    BUSY     |    7    | Busy signal detected.   | */
/* | ATP_NO_ANSWER_NB |  9  |  NO ANSWER  |    8    | '@' dial modifier was   | */
/* |                  |     |             |         | used, but remote        | */
/* |                  |     |             |         | ringing followed by 5   | */
/* |                  |     |             |         | seconds of silence was  | */
/* |                  |     |             |         | not detected before     | */
/* |                  |     |             |         | expiration of the       | */
/* |                  |     |             |         | connection timer.       | */
/* |__________________|_____|_____________|_________|_________________________| */
/* |                  |     |             |         |                         | */
/* |ATP_CONNECT_TXT_NB|  7  |   CONNECT   |    X    | A connection has been   | */
/* |                  |     |             |         | established.            | */
/* |__________________|_____|_____________|_________|_________________________| */
/* |                  |     |             |         |                         | */
/* |  ATP_PLUS_VGM_NB |  4  |    +VGM     |    X    | Unsolicited result code | */
/* |                  |     |             |         | issued by the AG to set | */
/* |                  |     |             |         | the microphone gain of  | */
/* |                  |     |             |         | the HS (See Headset     | */
/* |                  |     |             |         | Specification, Section  | */
/* |                  |     |             |         | 4.7.3).                 | */
/* |  ATP_PLUS_VGS_NB |  4  |    +VGS     |    X    | Unsolicited result code | */
/* |                  |     |             |         | issued by the AG to set | */
/* |                  |     |             |         | the speaker gain of the | */
/* |                  |     |             |         | HS (See Headset         | */
/* |                  |     |             |         | Specification, Section  | */
/* |                  |     |             |         | 4.7.3).                 | */
/* |__________________|_____|_____________|_________|_________________________| */
/*                                                                              */
/* Define the length of <CR><LF> headers of information responses (See ETS 300  */
/* 916 (GSM 07.07) Version 4.1 page 13).                                        */
#define ATP_RESULT_CODE_HEADER_LEN (0x02)

/* Text codes for result codes (verbose responses disabled).                    */
static const char ATP_RESULT_CODE_TXT_TABLE_V0[] = {'0', \
                          '1', \
                          '2', \
                          '3', \
                          '4', \
                          '6', \
                          '7', \
                          '8', \
                          'C', 'O', 'N', 'N', 'E', 'C', 'T', \
                          '+', 'V', 'G', 'M', \
                          '+', 'V', 'G', 'S', \
                          '\xFF'};

/* Text codes for result codes (verbose responses enabled).                     */
static const char ATP_RESULT_CODE_TXT_TABLE_V1[] = {'O', 'K', \
                          'C', 'O', 'N', 'N', 'E', 'C', 'T', \
                          'R', 'I', 'N', 'G', \
                          'N', 'O', ' ', 'C', 'A', 'R', 'R', 'I', 'E', 'R', \
                          'E', 'R', 'R', 'O', 'R', \
                          'N', 'O', ' ', 'D', 'I', 'A', 'L', 'T', 'O', 'N', 'E', \
                          'B', 'U', 'S', 'Y', \
                          'N', 'O', ' ', 'A', 'N', 'S', 'W', 'E', 'R', \
                          'C', 'O', 'N', 'N', 'E', 'C', 'T', \
                          '+', 'V', 'G', 'M', \
                          '+', 'V', 'G', 'S', \
                          '\xFF'};

/* Binary related codes.                                                        */
typedef enum
{
  ATP_OK_NB = (0x00000000),
  ATP_CONNECT_NB,
  ATP_RING_NB,
  ATP_NO_CARRIER_NB,
  ATP_ERROR_NB,
  ATP_NO_DIALTONE_NB = (0x00000006),
  ATP_BUSY_NB,
  ATP_NO_ANSWER_NB,
  ATP_CONNECT_TXT_NB,
  ATP_PLUS_VGM_NB,
  ATP_PLUS_VGS_NB,
  ATP_MAX_NB_OF_RESULT_CODES
} T_ATP_BINARY_RESULT_CODES;

/* Related offsets (verbose responses disabled).                                */
#define ATP_OK_OFFSET_V0 (0x00)
#define ATP_CONNECT_OFFSET_V0 (ATP_OK_OFFSET_V0 + 0x01)
#define ATP_RING_OFFSET_V0 (ATP_CONNECT_OFFSET_V0 + 0x01)
#define ATP_NO_CARRIER_OFFSET_V0 (ATP_RING_OFFSET_V0 + 0x01)
#define ATP_ERROR_OFFSET_V0 (ATP_NO_CARRIER_OFFSET_V0 + 0x01)
#define ATP_NO_DIALTONE_OFFSET_V0 (ATP_ERROR_OFFSET_V0 + 0x01)
#define ATP_BUSY_OFFSET_V0 (ATP_NO_DIALTONE_OFFSET_V0 + 0x01)
#define ATP_NO_ANSWER_OFFSET_V0 (ATP_BUSY_OFFSET_V0 + 0x01)
#define ATP_CONNECT_TXT_OFFSET_V0 (ATP_NO_ANSWER_OFFSET_V0 + 0x01)
#define ATP_PLUS_VGM_OFFSET_V0 (ATP_CONNECT_TXT_OFFSET_V0 + 0x07)
#define ATP_PLUS_VGS_OFFSET_V0 (ATP_PLUS_VGM_OFFSET_V0 + 0x04)
#define ATP_RESULT_CODE_LAST_OFFSET_V0 (ATP_PLUS_VGS_OFFSET_V0 + 0x04)

/* Related offsets (verbose responses enabled).                                 */
#define ATP_OK_OFFSET_V1 (0x00)
#define ATP_CONNECT_OFFSET_V1 (ATP_OK_OFFSET_V1 + 0x02)
#define ATP_RING_OFFSET_V1 (ATP_CONNECT_OFFSET_V1 + 0x07)
#define ATP_NO_CARRIER_OFFSET_V1 (ATP_RING_OFFSET_V1 + 0x04)
#define ATP_ERROR_OFFSET_V1 (ATP_NO_CARRIER_OFFSET_V1 + 0x0A)
#define ATP_NO_DIALTONE_OFFSET_V1 (ATP_ERROR_OFFSET_V1 + 0x05)
#define ATP_BUSY_OFFSET_V1 (ATP_NO_DIALTONE_OFFSET_V1 + 0x0B)
#define ATP_NO_ANSWER_OFFSET_V1 (ATP_BUSY_OFFSET_V1 + 0x04)
#define ATP_CONNECT_TXT_OFFSET_V1 (ATP_NO_ANSWER_OFFSET_V1 + 0x09)
#define ATP_PLUS_VGM_OFFSET_V1 (ATP_CONNECT_TXT_OFFSET_V1 + 0x07)
#define ATP_PLUS_VGS_OFFSET_V1 (ATP_PLUS_VGM_OFFSET_V1 + 0x04)
#define ATP_RESULT_CODE_LAST_OFFSET_V1 (ATP_PLUS_VGS_OFFSET_V1 + 0x04)

/* Related structures.                                                          */
typedef enum
{
  ATP_RESULT_CODE_NOT_DEFINED = (0x00000000),
  ATP_BASIC_RESULT_CODE,
  ATP_EXTENDED_RESULT_CODE,
  ATP_CONNECT_TXT_RESULT_CODE
} T_ATP_RESULT_CODE_FORMAT;

#define ATP_OK_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_CONNECT_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_RING_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_NO_CARRIER_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_ERROR_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_NO_DIALTONE_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_BUSY_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_NO_ANSWER_PARAM (ATP_BASIC_RESULT_CODE)
#define ATP_CONNECT_TXT_PARAM (ATP_CONNECT_TXT_RESULT_CODE)
#define ATP_PLUS_VGM_PARAM (ATP_EXTENDED_RESULT_CODE)
#define ATP_PLUS_VGS_PARAM (ATP_EXTENDED_RESULT_CODE)

#define ATP_RESULT_OFFSET_V0_COLUMN (0x00)
#define ATP_RESULT_OFFSET_V1_COLUMN (0x01)
#define ATP_RESULT_PARAM_COLUMN (0x02)

static const UINT8 ATP_RESULT_CODE_INFO[ATP_MAX_NB_OF_RESULT_CODES + 0x01][0x03] \
           = {{ATP_OK_OFFSET_V0, ATP_OK_OFFSET_V1, ATP_OK_PARAM}, \
            {ATP_CONNECT_OFFSET_V0, ATP_CONNECT_OFFSET_V1, ATP_CONNECT_PARAM}, \
            {ATP_RING_OFFSET_V0, ATP_RING_OFFSET_V1, ATP_RING_PARAM}, \
            {ATP_NO_CARRIER_OFFSET_V0, ATP_NO_CARRIER_OFFSET_V1, ATP_NO_CARRIER_PARAM}, \
            {ATP_ERROR_OFFSET_V0, ATP_ERROR_OFFSET_V1, ATP_ERROR_PARAM}, \
            {ATP_NO_DIALTONE_OFFSET_V0, ATP_NO_DIALTONE_OFFSET_V1, ATP_RESULT_CODE_NOT_DEFINED}, \
            {ATP_NO_DIALTONE_OFFSET_V0, ATP_NO_DIALTONE_OFFSET_V1, ATP_NO_DIALTONE_PARAM}, \
            {ATP_BUSY_OFFSET_V0, ATP_BUSY_OFFSET_V1, ATP_BUSY_PARAM}, \
            {ATP_NO_ANSWER_OFFSET_V0, ATP_NO_ANSWER_OFFSET_V1, ATP_NO_ANSWER_PARAM}, \
            {ATP_CONNECT_TXT_OFFSET_V0, ATP_CONNECT_TXT_OFFSET_V1, ATP_CONNECT_TXT_PARAM}, \
            {ATP_PLUS_VGM_OFFSET_V0, ATP_PLUS_VGM_OFFSET_V1, ATP_PLUS_VGM_PARAM}, \
            {ATP_PLUS_VGS_OFFSET_V0, ATP_PLUS_VGS_OFFSET_V1, ATP_PLUS_VGS_PARAM}, \
            {ATP_RESULT_CODE_LAST_OFFSET_V0, ATP_RESULT_CODE_LAST_OFFSET_V1, ATP_RESULT_CODE_NOT_DEFINED}};

/* Define the maximum extended result code parameter length:                    */
/*    - '=' + <value> (up to 65535).                                            */
#define MAX_SINGLE_RESULT_CODE_VALUE_LEN (0x06)

/* Define the structure associated with extended result codes. The format of    */
/* such result codes is <result_code>[=<value>] (See Headset Specification,     */
/* Section 4.7.3). <value> shall consist of a numeric constant.                 */
typedef struct
{
  UINT16 value;
} T_ATP_SINGLE_RESULT_CODE_VALUE;

/* Define all extended syntax result codes.                                     */
/*  __________________________________________________________________________  */
/* |                  |     |             |         |                         | */
/* |    Binary code   | Len | Result code | Numeric |       Description       | */
/* |__________________|_____|_____________|_________|_________________________| */
/* |                  |     |             |         |                         | */
/* |  ATP_PLUS_VGM_NB |  4  |    +VGM     |    X    | Unsolicited result code | */
/* |                  |     |             |         | issued by the AG to set | */
/* |                  |     |             |         | the microphone gain of  | */
/* |                  |     |             |         | the HS (See Headset     | */
/* |                  |     |             |         | Specification, Section  | */
/* |                  |     |             |         | 4.7.3).                 | */
/* |  ATP_PLUS_VGS_NB |  4  |    +VGS     |    X    | Unsolicited result code | */
/* |                  |     |             |         | issued by the AG to set | */
/* |                  |     |             |         | the speaker gain of the | */
/* |                  |     |             |         | HS (See Headset         | */
/* |                  |     |             |         | Specification, Section  | */
/* |                  |     |             |         | 4.7.3).                 | */
/* |__________________|_____|_____________|_________|_________________________| */
typedef T_ATP_SINGLE_RESULT_CODE_VALUE T_ATP_PLUS_VGM_PARAM;
typedef T_ATP_SINGLE_RESULT_CODE_VALUE T_ATP_PLUS_VGS_PARAM;

/* Define the maximum number of characters appearing in the <text>.             */
/*    - '=' + <value> (up to 65535).                                            */
#define MAX_CONNECT_TXT_LEN (MAX_SINGLE_RESULT_CODE_VALUE_LEN)

/* Define the structure associated with CONNECT<text> result code whose format  */
/* is CONNECT <text> (See ITU-T Recommendation V.250 ter page 11). <text>       */
/* should specify the baudrate (1200, 2400, 4800, 9600 ...).                    */
/*  __________________________________________________________________________  */
/* |                  |     |             |         |                         | */
/* |    Binary code   | Len | Result code | Numeric |       Description       | */
/* |__________________|_____|_____________|_________|_________________________| */
/* |                  |     |             |         |                         | */
/* |ATP_CONNECT_TXT_NB|  7  |   CONNECT   |    X    | A connection has been   | */
/* |                  |     |             |         | established.            | */
/* |__________________|_____|_____________|_________|_________________________| */
typedef T_ATP_SINGLE_RESULT_CODE_VALUE T_ATP_CONNECT_TXT_PARAM;

/************************************ MACROS ************************************/
/*                                                                              */
/* Define a macro used to compare the first "count" characters of the two       */
/* buffers "reference" and "buffer". The comparison is not case sensitive. Note */
/* that the "reference" must be in all caps. Therefore, the gap between         */
/* alphabetic lower-case characters and their upper-case equivalents is 0x20    */
/* (See CCITT T.50 (1992), International Reference Alphabet).                   */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_MEM_I_CMP (T_ATP_TXT_CMD  reference,                                   */
/*                  T_ATP_TXT_CMD  buffer,                                      */
/*                  UINT8          count,                                       */
/*                  BOOLEAN        *equal);                                     */
#define ATP_MEM_I_CMP(reference, \
            buffer, \
            count, \
            equal) \
    { \
      UINT8 nb_byte = 0x00; \
      while ((nb_byte < (count)) && \
           (((buffer)[nb_byte] == (reference)[nb_byte]) || \
          (((buffer)[nb_byte] >= 'a') && \
           ((buffer)[nb_byte] <= 'z') && \
           ((reference)[nb_byte] == ((buffer)[nb_byte] - '\x20'))))) \
      { \
        nb_byte++; \
      } \
      *(equal) = (BOOLEAN) ((nb_byte == (count)) ? (TRUE) : (FALSE)); \
    }

/* Define a macro used to compare the first "count" characters of the two       */
/* buffers "reference" and "buffer". The comparison is not case sensitive.      */
/* Besides, space characters are ignored and may be used freely for formatting  */
/* purposes (See ITU-T Recommendation V.250 ter page 4). Note that the          */
/* "reference" must be in all caps. Therefore, the gap between alphabetic       */
/* lower-case characters and their upper-case equivalents is 0x20 (See CCITT    */
/* T.50 (1992), International Reference Alphabet).                              */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_MEM_SP_I_CMP (T_ATP_TXT_CMD  reference,                                */
/*                     T_ATP_TXT_CMD  buffer,                                   */
/*                     UINT8          *count,                                   */
/*                     BOOLEAN        *equal);                                  */
#define ATP_MEM_SP_I_CMP(reference, \
             buffer, \
             count, \
             equal) \
    { \
      UINT8 nb_byte_ref = 0x00; \
      UINT8 nb_byte_buf = 0x00; \
      while ((nb_byte_ref < *(count)) && \
           (((buffer)[nb_byte_buf] == '\x20') || \
          ((buffer)[nb_byte_buf] == (reference)[nb_byte_ref]) || \
          (((buffer)[nb_byte_buf] >= 'a') && \
           ((buffer)[nb_byte_buf] <= 'z') && \
           ((reference)[nb_byte_ref] == ((buffer)[nb_byte_buf] - '\x20'))))) \
      { \
        if ((buffer)[nb_byte_buf++] == '\x20') \
        { \
          continue; \
        } \
        nb_byte_ref++; \
      } \
      *(equal) = (BOOLEAN) ((nb_byte_ref == *(count)) ? (TRUE) : (FALSE)); \
      *(count) = nb_byte_buf; \
    }

/************************ MACROS DEDICATED TO AT COMMANDS ***********************/
/*                                                                              */
/* Define a macro used to get the <number> of basic syntax commands. Note that  */
/* <number> may be a string of one or more characters from "0" through "9"      */
/* representing the decimal integer value (See ITU-T Recommendation V.250 ter   */
/* page 5). Besides, if <number> is missing, the "initial_value" is assumed.    */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_NUMBER (T_ATP_TXT_CMD  cmd_line_p,                                 */
/*                   UINT16         *number_p,                                  */
/*                   const char     initial_value);                             */
#define ATP_GET_NUMBER(cmd_line_p, \
             number_p, \
             initial_value) \
    { \
      char nb_value_p[0x05] = {'\x00','\x00','\x00','\x00','\x00'}; \
      UINT8 nb_digit = 0x00; \
      for (*nb_value_p = initial_value; \
         ((*(cmd_line_p) >= '0') && (*(cmd_line_p) <= '9')) || \
         (*(cmd_line_p) == ' '); \
         (cmd_line_p)++) \
      { \
        if (((*(cmd_line_p) == '0') && (nb_digit == 0x00)) || \
          (*(cmd_line_p) == ' '))\
        { \
          continue; \
        } \
        nb_value_p[nb_digit++] = *(cmd_line_p); \
      } \
      *(number_p) = (UINT16) atoi (nb_value_p); \
    }

/* Define a macro used to get the <dial_string> of dial (See ITU-T              */
/* Recommendation V.250 ter page 31). All characters appearing on the same      */
/* command line after D are considered part of the call addressing information  */
/* to be signalled to the network, or modifiers used to control the signalling  */
/* process, up to a semicolon character or the end of the command line. If the  */
/* <dial_string> is terminated by a semicolon, the DCE does not start the call  */
/* origination procedure, but instead returns to command state after completion */
/* of the signalling of call addressing information to the network.             */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_DIAL_STRING (T_ATP_TXT_CMD  cmd_line_p,                            */
/*                        T_ATP_TXT_CMD  dial_string_p,                         */
/*                        UINT8          *dial_string_length_p,                 */
/*                        UINT8          *call_type_p,                          */
/*                        const char     termination_char);                     */
#define ATP_GET_DIAL_STRING(cmd_line_p, \
              dial_string_p, \
              dial_string_length_p, \
              call_type_p, \
              termination_char) \
    { \
      *(call_type_p) = DATA_CALL; \
      *(dial_string_length_p) = 0x00; \
      while (*(cmd_line_p) != termination_char) \
      { \
        if (*(cmd_line_p) == ';') \
        { \
          *(call_type_p) = VOICE_CALL; \
          break; \
        } \
        (dial_string_p)[(*(dial_string_length_p))++] = *(cmd_line_p)++; \
      } \
    }

/* Define a macro used to get operators such as "=", "?" or "=?".               */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_OPERATOR (T_ATP_TXT_CMD            cmd_line_p,                     */
/*                     T_ATP_EXTENDED_OPERATOR  *s_operator_p);                 */
#define ATP_GET_OPERATOR(cmd_line_p, \
             s_operator_p) \
    { \
      for (*(s_operator_p) = 0x00; \
         ((*(cmd_line_p) == '=') || (*(cmd_line_p) == '?') || \
         (*(cmd_line_p) == ' ')); \
         (cmd_line_p)++) \
      { \
        switch (*(cmd_line_p)) \
        { \
          case '?': \
            { \
              *(s_operator_p) += 0x01; \
              break; \
            } \
          case '=': \
            { \
              *(s_operator_p) += 0x02; \
              break; \
            } \
          default: \
            { \
              break; \
            } \
        } \
      } \
    }

/* Define a macro used to get the <value> of S-parameters. Note that <value>    */
/* shall consist of a decimal constant (See ITU-T Recommendation V.250 ter page */
/* 6).                                                                          */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_PARAMETER_VALUE (T_ATP_TXT_CMD            cmd_line_p,              */
/*                            T_ATP_EXTENDED_OPERATOR  *s_operator_p,           */
/*                            UINT16                   *value_p);               */
#define ATP_GET_PARAMETER_VALUE(cmd_line_p, \
                s_operator_p, \
                value_p) \
    { \
      ATP_GET_OPERATOR((cmd_line_p), \
               (s_operator_p)); \
      ATP_GET_NUMBER((cmd_line_p), \
               (value_p), \
               '0'); \
    }

/* Define a macro used to proceed with subsequent commands that might follow an */
/* extended syntax command (See ITU-T Recommendation V.250 ter page 9).         */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_ADDITIONAL_CMD (T_ATP_TXT_CMD  cmd_line_p,                         */
/*                           const char     termination_char);                  */
#define ATP_GET_ADDITIONAL_CMD(cmd_line_p, \
                 termination_char) \
    { \
      while (*(cmd_line_p) != termination_char) \
      { \
        if (*(cmd_line_p)++ == ';') \
        { \
          break; \
        } \
      } \
    }

/* Define a macro used to get the <value> of extended syntax commands. Note     */
/* that <value> shall consist of either a numeric constant or a string constant */
/* (See ITU-T Recommendation V.250 ter page 6). However, only numeric constants */
/* are supported for now.                                                       */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_VALUE (T_ATP_TXT_CMD            cmd_line_p,                        */
/*                  T_ATP_EXTENDED_OPERATOR  *extended_operator_p,              */
/*                  UINT16                   *value_p,                          */
/*                  const char               termination_char);                 */
#define ATP_GET_VALUE(cmd_line_p, \
            extended_operator_p, \
            value_p, \
            termination_char) \
    { \
      ATP_GET_OPERATOR((cmd_line_p), \
               (extended_operator_p)); \
      ATP_GET_NUMBER((cmd_line_p), \
               (value_p), \
               '0'); \
      ATP_GET_ADDITIONAL_CMD((cmd_line_p), \
                   (termination_char)); \
    }

/* Define a macro used to get the <keys> of keypad control command. Note that   */
/* <keys> shall consist of a string constant (See ETS 300 916 (GSM 07.07)       */
/* Version 5.8.1 page 62). Note also that <keys> is a string of characters      */
/* representing keys (See See ETS 300 916 (GSM 07.07) Version 5.8.1 page 62).   */
/* Colon character followed by one character can be used to indicate a          */
/* manufacturer specific key not listed here. All characters from a semicolon   */
/* character to the next single semicolon character are treated as alpha        */
/* entries and are not converted to key equivalents. All semicolon characters   */
/* inside alpha entries should be duplicated in the DTE. Pause characters "W"   */
/* and "w" can be used to pause between key pressings for a time specified by   */
/* <pause>.                                                                     */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_KEYS (T_ATP_TXT_CMD             cmd_line_p,                        */
/*                 T_ATP_AT_PLUS_CKPD_PARAM  *ckpd_param_p,                     */
/*                 const char                termination_char);                 */
#define ATP_GET_KEYS(cmd_line_p, \
           ckpd_param_p, \
           termination_char) \
    { \
      ATP_GET_NUMBER((cmd_line_p), \
               &((ckpd_param_p)->keys[0x00]), \
               '0'); \
      (ckpd_param_p)->nb_keys = 0x01; \
      ATP_GET_ADDITIONAL_CMD((cmd_line_p), \
                   (termination_char)); \
    }
/*    { \ */
/*      UINT8  nb_keys = 0x00; \ */
/*      while (*(cmd_line_p) != termination_char) \ */
/*      { \ */
/*        if (*(cmd_line_p)++ == '\"') \ */
/*        { \ */
/*          break; \ */
/*        } \ */
/*      } \ */
/*      while (*(cmd_line_p) != termination_char) \ */
/*      { \ */
/*        switch (*(cmd_line_p)++) \ */
/*        { \ */
/*          case ' ': \ */
/*            { \ */
/*              continue; \ */
/*            } \ */
/*          case ';': \ */
/*            { \ */
/*              ATP_GET_NUMBER((cmd_line_p), \ */
/*                       &((ckpd_param_p)->keys[nb_keys]), \ */
/*                       '0'); \ */
/*              nb_keys++; \ */
/*              while (*(cmd_line_p) != termination_char) \ */
/*              { \ */
/*                if (*(cmd_line_p)++ == ';') \ */
/*                { \ */
/*                  break; \ */
/*                } \ */
/*              } \ */
/*              continue; \ */
/*            } \ */
/*          case ':': \ */
/*            { \ */
/*              while (*(cmd_line_p) != termination_char) \ */
/*              { \ */
/*                if (*(cmd_line_p)++ == ':') \ */
/*                { \ */
/*                  break; \ */
/*                } \ */
/*              } \ */
/*              continue; \ */
/*            } \ */
/*          case '\"': \ */
/*            { \ */
/*              break; \ */
/*            } \ */
/*          default: \ */
/*            { \ */
/*              (ckpd_param_p)->keys[nb_keys] = (UINT16) (*(cmd_line_p - 0x01)); \ */
/*              nb_keys++; \ */
/*              continue; \ */
/*            } \ */
/*        } \ */
/*        break; \ */
/*      } \ */
/*      (ckpd_param_p)->nb_keys = nb_keys; \ */
/*    } */

/* Define a macro used to proceed with subsequent numeric and string values     */
/* that might appear in a compound value (See ITU-T Recommendation V.250 ter    */
/* page 7).                                                                     */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_ADDITIONAL_VALUE (T_ATP_TXT_CMD  cmd_line_p,                       */
/*                             const char     termination_char);                */
#define ATP_GET_ADDITIONAL_VALUE(cmd_line_p, \
                 termination_char) \
    { \
      while (*(cmd_line_p) != termination_char) \
      { \
        switch (*(cmd_line_p)) \
        { \
          case ',': \
            { \
              cmd_line_p++; \
            } \
          case ';': \
            { \
              break; \
            } \
          default: \
            { \
              cmd_line_p++; \
              continue; \
            } \
        } \
        break; \
      } \
    }

/* Define a macro used to get the <keys>,<time> and <pause> of keypad control   */
/* command. Note that <keys> shall consist of a string constant and <time> and  */
/* <pause> shall consist of numeric constants in tenths of a second (See ETS    */
/* 300 916 (GSM 07.07) Version 5.8.1 page 62).                                  */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_CKPD_PARAM (T_ATP_TXT_CMD             cmd_line_p,                  */
/*                       T_ATP_AT_PLUS_CKPD_PARAM  *ckpd_param_p,               */
/*                       const char                termination_char);           */
#define ATP_GET_CKPD_PARAM(cmd_line_p, \
               ckpd_param_p, \
               termination_char) \
    { \
      ATP_GET_OPERATOR((cmd_line_p), \
               &((ckpd_param_p)->extended_operator)); \
      ATP_GET_KEYS((cmd_line_p), \
             (ckpd_param_p), \
             (termination_char)); \
      ATP_GET_ADDITIONAL_VALUE((cmd_line_p), \
                   (termination_char)); \
      ATP_GET_NUMBER((cmd_line_p), \
               &((ckpd_param_p)->time), \
               '0'); \
      ATP_GET_ADDITIONAL_VALUE((cmd_line_p), \
                   (termination_char)); \
      ATP_GET_NUMBER((cmd_line_p), \
               &((ckpd_param_p)->pause), \
               '0'); \
      ATP_GET_ADDITIONAL_CMD((cmd_line_p), \
                   (termination_char)); \
    }

/************************ MACRO DEDICATED TO RESULT CODES ***********************/
/*                                                                              */
/* Define a macro used to get the <value> associated with extended result codes */
/* (See Headset Specification, Section 4.7.3). Note that <value> shall consist  */
/* of a numeric constant.                                                       */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_RESULT_CODE_VALUE (T_ATP_TXT_CMD  cmd_line_p,                      */
/*                              UINT16         *value_p);                       */
#define ATP_GET_RESULT_CODE_VALUE(cmd_line_p, \
                  value_p) \
    { \
      UINT8 result_operator = 0x00; \
      ATP_GET_OPERATOR((cmd_line_p), \
               &result_operator); \
      ATP_GET_NUMBER((cmd_line_p), \
               (value_p), \
               '0'); \
    }

/* Define a macro used to get the <text> associated with CONNECT <text> result  */
/* code (See ITU-T Recommendation V.250 ter page 11).                           */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_CONNECT_TXT (T_ATP_TXT_CMD  cmd_line_p,                            */
/*                        UINT16         *value_p);                             */
#define ATP_GET_CONNECT_TXT(cmd_line_p, \
              value_p) \
    { \
      ATP_GET_NUMBER((cmd_line_p), \
               (value_p), \
               '0'); \
    }

/************************** MACRO DEDICATED TO RAW DATA *************************/
/*                                                                              */
/* Define a macro used to get the length of unknown commands from raw data,     */
/* <CR> not included. Note that all characters appearing on the same command    */
/* line are considered part of the unknown command, up to the end of the        */
/* command line.                                                                */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_UNKNOWN_CMD_LEN (T_ATP_TXT_CMD  cmd_line_p,                        */
/*                            UINT8          *cmd_length_p);                    */
/*                            const char     termination_char);                 */
#define ATP_GET_UNKNOWN_CMD_LEN(cmd_line_p, \
                cmd_length_p, \
                termination_char) \
    { \
      for (*(cmd_length_p) = 0x00; \
         (cmd_line_p)[*(cmd_length_p)] != termination_char; \
         (*(cmd_length_p))++) \
      { \
      } \
    }

/* Define a macro used to get the length of unknown AT commands from raw data,  */
/* <CR> or semicolon not included. Note that all characters appearing on the    */
/* same command line are considered part of the unknown command, up to a        */
/* semicolon character or the end of the command line.                          */
/*                                                                              */
/* Prototype:                                                                   */
/*   ATP_GET_UNKNOWN_AT_CMD_LEN (T_ATP_TXT_CMD  cmd_line_p,                     */
/*                               UINT8          *cmd_length_p);                 */
/*                               const char     termination_char);              */
#define ATP_GET_UNKNOWN_AT_CMD_LEN(cmd_line_p, \
                   cmd_length_p, \
                   termination_char) \
    { \
      for (*(cmd_length_p) = 0x00; \
         ((cmd_line_p)[*(cmd_length_p)] != termination_char) && \
         ((cmd_line_p)[*(cmd_length_p)] != ';'); \
         (*(cmd_length_p))++) \
      { \
      } \
    }

#endif

