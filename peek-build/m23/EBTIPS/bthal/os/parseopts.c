/***************************************************************************
 *
 * File:
 *     $Workfile:parseopts.c$ for XTNDAccess Blue SDK, Version 2.0.2
 *     $Revision:43$
 *
 * Description:
 *     This file contains the code which parses and exports the command
 *     line options for the stack.
 *
 * Created:
 *     October 7, 1999
 *
 * Copyright 1999-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <osapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parseopts.h>


/*---------------------------------------------------------------------------
 *
 * Global variables
 */
//static UserOpts opts = { 1, 115, RADIO_TYPE_ESI_INET}; /* ESI INET (TCP/IP sockets) */
static UserOpts opts = { 1, 115, RADIO_TYPE_TI_UART}; /* TI UART */

static char     errMessage[256];


/*---------------------------------------------------------------------------
 * ParseStandardOptions()
 *
 *     Parses option pointed to by 'opt' parameter. If the parameter is
 * recognized, the 'opt' pointer is advanced past the option.
 */
BOOL ParseStandardOptions(char **opt)
{
    int         speed;
    I8          i, j;
    char       *cmd = *opt;
    BOOL        done = FALSE;

    *errMessage = 0;

    switch (*cmd++) {
    case '/':
    case '-':
        /* Look for a normal switch */
        switch (*cmd++) {
        case 'p':
            opts.portNum = atoi(cmd);

            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;

        case 's':
            if (strncmp(cmd, "niff", 4) == 0) {
                /* It's -sniff not a speed setting. */
                opts.startSniffer = TRUE;
                while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
                break;
            }

            i = 0;
            while (!done) {
                if (!((cmd[i] >= '0') && (cmd[i] <= '9'))) {
                    if (cmd[i] == '.') {
                        /* Decimal point */
                        j = i + 1;
                        while ((cmd[j] >= '0') && (cmd[j] <= '9')) {
                            if (j == i + 1) {
                                /* Only one digit after decimal */
                                cmd[j - 1] = cmd[j];
                            }
                            cmd[j] = ' ';
                            j++;
                        }
                    } else if (cmd[i] && (cmd[i] != ' ')) {
                        sprintf(errMessage, "Invalid speed '%s'",cmd);
                        *opt = cmd;
                        return FALSE;
                    }
                    done = TRUE;
                } 
                i++;
            }

            speed = atoi(cmd);
            opts.speed = speed;
            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;
            
        case 't':
            if (strncmp(cmd, "TI_UART", 2) == 0) {
                opts.radioType = RADIO_TYPE_TI_UART;
            } else if (strncmp(cmd, "ESI_INET", 3) == 0) {
                opts.radioType = RADIO_TYPE_ESI_INET;
            } else {
                sprintf(errMessage, "Invalid radio type '%s'",cmd);
                *opt = cmd;
                return FALSE;
            }
            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;



        default:
            /* Unrecognized - or / switch */
            sprintf(errMessage,"Unrecognized command line switch '%c'",*(cmd-1));
            return FALSE;
        }
        break;
        
    default:
        /* Unrecognized option */
        sprintf(errMessage,"Unrecognized command line option %s", cmd-1);
        return FALSE;
    }

    /* We survived the parse, return success */
    *opt = cmd;
    return TRUE;
}

/*---------------------------------------------------------------------------
 * getPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getPortNumOption(void)
{
    return opts.portNum;
}

/*---------------------------------------------------------------------------
 * getSpeedOption()
 *     Returns the user-selected bps rate.
 */
U16 getSpeedOption(void)
{
    return opts.speed;
}

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the user-selected radio type.
 */
U16 getRadioType(void)
{
    return opts.radioType;
}

/*---------------------------------------------------------------------------
 * getSnifferOption()
 *     Returns the user-selected sniffer on/off state.
 */
BOOL getSnifferOption(void)
{
    return opts.startSniffer;
}

/*---------------------------------------------------------------------------
 * getErrorMesage()
 *     Returns the user-selected bps rate.
 */
const char *getErrorMessage(void)
{
    return errMessage;
}

/*---------------------------------------------------------------------------
 * getStandardOptions()
 *     Returns a string which describes the standard options
 */
const char *getStandardOptions(void)
{
    return 
           "   -p<port num>\tBluetooth: COM port number\t\t(default=1)\n"
           "   -s<port speed>\tBluetooth: 9, 38, 57, 115, 256 <kbps>\t(default=115)\n"
           "   -t<radio type>\tBluetooth: TI_UART\n"
           "   \t\tESI_INET\t\t(default=ESI_INET)\n"
           "   -sniff\t\tOpen sniffer at startup\n"
           "";
}


