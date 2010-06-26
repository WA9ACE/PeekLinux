/*****************************************************************************/
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Copyright (c) 2002                                                        */
/* Matsushita Communication Industrial, MMCD USA                             */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* File Name:                                                                */
/*     l1gtt_baudot_functions.h                                              */
/*                                                                           */
/* Purpose:                                                                  */
/*     This is the header file for "l1gtt_baudot_functions.c".               */
/*                                                                           */
/*                                                                           */
/* Warnings:                                                                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*** Maintenance *********************************************************/
/*                                                                       */
/*                                                                       */
/*                 02/11/2002   initial version created       Laura Ning */
/*                                                                       */
/*************************************************************************/
#ifndef _BAUDOT_DECODE_H_
#define BAUDOT_DECODE_H

#include "ctm_typedefs.h"

/*
*******************************************************************************
*                         DEFINITIONS
*******************************************************************************
*/

/* definitions (might be of global interest) */
#define BAUDOT_NUM_INFO_BITS    5 /* number of info bits per TTY character  */
#define BAUDOT_SHIFT_FIGURES   27 /* code of shift to figures symbol        */
#define BAUDOT_SHIFT_LETTERS   31 /* code of shift to letters symbol        */

#define BAUDOT_LOGICAL_1  1
#define BAUDOT_LOGICAL_0  -1
#define BAUDOT_NON_TTY   0
#define BAUDOT_START_BIT  -1
#define BAUDOT_STOP_BIT   1

#define BAUDOT_STOP_OUTPUT  2 /* STOP bit as the output of baudot_encode() to DSP */

/* definations (be only of local interest)   */

#define START_OF_STOP_BIT  26

#define DETECT_WINDOW_WIDTH  4   /* A sliding window is used to detect start,stop and data bits */
#define BAUDOT_SAMPLE_DURATION 3   /* The duration to meet to determine a valid baudot bit        */

/* A temporary assumption of the input buffer size, to be    */
/* replaced later when TI has the actual buffer size defined */
#define MAX_WORDS_OF_BUFFER1      40  /* Number of the 5ms-bits it can contain  */
#define TIME_INTERVAL_OF_BUFFER1  200 /* in the unit of ms                      */
#define MAX_WORDS_ONE_TTYCODE   36  /* Number of 5ms-bits a TTY character has */

#define NUM_STOP_BITS_TX        2    /* number of stop bits per character    */

/* ******************************************************************/
/* Type definitions for variables that contain all states of the    */
/* Baudot encoder                                                   */
/* ******************************************************************/
typedef struct 
{
  WORD16       cntCharsSinceLastShift;
  BOOL           inFigureMode;
  BOOL           tailBitsGenerated;
//  fifo_state_t   fifo_state;
} baudot_encode_state_t;

/****************************************************************************/
/* convertChar2ttyCode()                                                    */
/* *********************                                                    */
/* Conversion from character into tty code                                  */
/*                                                                          */
/* input variables:                                                         */
/* - inChar       charcater that shall be converted                         */
/*                                                                          */
/* return value:  baudot code of the input character                        */
/*                or -1 in case that inChar is not valid (e.g. inChar=='\0')*/
/*                                                                          */
/* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/02/17 */
/****************************************************************************/

WORD16 convertChar2ttyCode(char inChar);



/****************************************************************************/
/* convertTTYcode2char()                                                    */
/* *********************                                                    */
/* Conversion from tty code into character                                  */
/*                                                                          */
/* input variables:                                                         */
/* - ttyCode      Baudot code (must be within the range 0...63)             */
/*                or -1 if there is nothing to convert                      */
/*                                                                          */
/* return value:  character (or '\0' if ttyCode is not valid)               */
/*                                                                          */
/* Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/02/17 */
/****************************************************************************/

char convertTTYcode2char(WORD16 ttyCode);

/* prototype declarations */
void init_baudot_encode(baudot_encode_state_t* state);

void baudot_encode(WORD16 inputTTYcode,
       fifo_state_t* ptrOutFifoState,
       baudot_encode_state_t* state);
  
#endif
