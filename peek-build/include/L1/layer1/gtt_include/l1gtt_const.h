/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_CONST.C
 *
 *        Filename l1gtt_const.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/


//----------------------------------------
// MCU<->DSP communication bit field.
//----------------------------------------
// NDB area 
// bit in d_audio for tty features
#define B_GTT_START   (TRUE_L << 4)
#define B_GTT_STOP    (TRUE_L << 5)

//-------------------------------
// GTT background task status
//-------------------------------

#define GTT_ACTIVATED   1
#define GTT_IDLE        0

//----------------------------
// GTT DSP customizable values
//----------------------------

#define C_TTY_MOD_NORM              0x0080    // Sets the amplitude of the TTY modulator output
#define C_TTY_NUM_SAMPLES_STOP_BIT  0x0160    // Sets the number of Stop bits inserted by
                                              // the Baudot Modulator after each character
#define C_CTM_DETECT_SHIFT          0x0000    // Shift of the threshold used for detection by the CTM estimator
#define C_CTM_MOD_NORM              0x80
#define C_CTM_MOD_0_50              0x40
#define C_CTM_MOD_0_75              0x60
#define C_CTM_MOD_0_25              0x20
#define C_TTY_OFFSET_NORM           0x0003C   //60
#define C_TTY_THRES_START_BIT       0x00008   // 8
#define C_TTY_THRES_DIFF            0x008FC   // 2300
#define C_TTY_DURA_START_DETECT     0x00046   // 70
#define C_TTY_START_BIT_THRES       0x0FEB8   // -328

#define C_DL_BYPASS_MUTE_ENABLE     (0x0001)
#define C_DL_BYPASS_MUTE_DISABLE    (0x0000)

//-------------------------
// FIFO constants
//-------------------------

// FIFO size
// Note: Practical size is equal to acutual size + 1.
#define FIFO_1_size 0x2  //2
#define FIFO_2_size 0x78 //120
#define FIFO_3_size 0x3C //60  
#define FIFO_4_size 0x1C //28 

// Header size 
#define HEADER_size (sizeof(T_TTY_FIFO_struct)/2)

/***********************************************************************/
/*            ___                *                  ___                */
/* @dsp 10BE | R |               *   @mcu FFD0117C | R |               */
/*           | W |               *                 | W |               */
/*           | S |               *                 | S |               */
/*           | L |               *                 | L |               */
/*           | x |               *                 | x |               */
/*   FIFO 1  | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           | x |               *                 | x |               */
/*           |___|               *                 |___|               */
/*            ___                *                  ___                */
/* @dsp 10DF | R |               *   @mcu FFD011BE | R |               */
/*           | . |               *                 | . |               */
/*   FIFO 2  | . |               *     FIFO 2      | . |               */
/*           |___|               *                 |___|               */
/*            ___                *                  ___                */
/* @dsp 1120 | R |               *   @mcu FFD01240 | R |               */
/*           | . |               *                 | . |               */
/*   FIFO 3  | . |               *     FIFO 3      | . |               */
/*           |___|               *                 |___|               */
/*            ___                *                  ___                */
/* @dsp 1161 | R |               *   @mcu FFD012C2 | R |               */
/*           | . |               *                 | . |               */
/*   FIFO 4  | . |               *     FIFO 4      | . |               */
/*           |___|               *                 |___|               */
/*                  *                                     */
/***********************************************************************/

//FIFO @dress dsp
#if (DSP == 38) || (DSP == 39)
  #define FIFO_3_dsp 0x1550 // corresponds to the speech recognition buffer
#elif (DSP == 37)
  #define FIFO_3_dsp 0x1605 // corresponds to the speech recognition buffer
#else
  #define FIFO_3_dsp 0x10BE // corresponds to the speech recognition buffer
#endif
#define FIFO_4_dsp (FIFO_3_dsp + HEADER_size + FIFO_3_size + 1)
#define FIFO_2_dsp (FIFO_4_dsp + HEADER_size + FIFO_4_size + 1)
#define FIFO_1_dsp (FIFO_2_dsp + HEADER_size + FIFO_2_size + 1)

//FIFO @dress mcu
#if (DSP == 38) || (DSP == 39)
  #define FIFO_3_mcu 0xFFD01AA0 // corresponds to the speech recognition buffer
#elif (DSP == 37)
  #define FIFO_3_mcu 0xFFD01C0A // corresponds to the speech recognition buffer
#else
  #define FIFO_3_mcu 0xFFD0117C // corresponds to the speech recognition buffer
#endif
#define FIFO_4_mcu (FIFO_3_mcu + ((HEADER_size + FIFO_3_size + 1) * 2))
#define FIFO_2_mcu (FIFO_4_mcu + ((HEADER_size + FIFO_4_size + 1) * 2))
#define FIFO_1_mcu (FIFO_2_mcu + ((HEADER_size + FIFO_2_size + 1) * 2))

// Return status
#define TTY_FIFO_OK   0 
#define TTY_FIFO_OVERFLOW 1 
#define TTY_FIFO_UNDERFLOW 2 
#define TTY_FIFO_WRONG_COMMAND 3

/* GTT Event group defines */
#define DATA_AVAIL_EVENT            0x00000001  // BIT_0
#define INIT_EVENT                  0x00000002  // BIT_1
#define EXIT_EVENT                  0x00000004  // BIT_2

#define DSP_API_START               0x800

/* constant definitions */
/* Time interval to check for incoming data, in ms */
#define  TIME_INTERVAL_OF_INPUTS    120  
/* Max number of CTM sample inputs    */
#define  MAX_CTM_SAMPLES_IN         (TIME_INTERVAL_OF_INPUTS * 2)/5  
/* Max number of Baudot sample inputs */  
#define  MAX_BAUDOT_SAMPLES_IN      (TIME_INTERVAL_OF_INPUTS * 1)/5   
/* Max number of TTY code from inputs */ 
#define  MAX_TTY_CODE_IN            (TIME_INTERVAL_OF_INPUTS / 176 + 2) 
/* Max number of CTM sample outputs   */
#define  MAX_CTM_SAMPLES_OUT        MAX_CTM_SAMPLES_IN   
/* Max number of Baudot bits outputs  */    
#define  MAX_BAUDOT_BITS_OUT        2*(MAX_TTY_CODE_IN)* 8    

/* Number of processing cycles before the next Enquiry burst */
#define  ENQUIRY_BURST_TIMEOUT      (1320/5)

#define UWORD16_MAX 65535
#define WORD16_MAX  32767

#define WORD32_MAX  2147483647L

/* GTT loops for test: TST_CLOSE_GTT_LOOP_REQ message */
#define TTY_LOOP 0x0001   // Close TTY loop (TTY modulator -> TTY estimator)
#define CTM_LOOP 0x0002   // Close CTM loop (CTM samples : speech encoder -> speech decoder) 

#if (L2_L3_SIMUL)
  #define CTM_TX_INPUT   1 // input on CTM transmitter
  #define TTY_ENC_INPUT  2 // input on Baudot encoder
  #define CTM_RX_INPUT   3 // input on CTM receiver (simulation only)
  #define TTY_DEC_INPUT  4 // input on Baudot decoder (simulation only)
#endif
