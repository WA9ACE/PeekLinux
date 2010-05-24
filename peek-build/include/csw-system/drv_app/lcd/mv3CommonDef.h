/*-------------------------------------------------------------------
File name : mv3CommonDef.h

MV400 middle level camera control routines

(C) Copyright MtekVision, Inc. 2003~2004 All right reserved.
  Confidential Information

All parts of the MtekVision Program Source are protected by copyright law 
and all rights are reserved. 
This documentation may not, in whole or in part, be copied, photocopied, 
reproduced, translated, or reduced to any electronic medium or machine 
readable form without prior consent, in writing, from the MtekVision. 

Last modification : 2005. 03. 05
----------------------------------------------------------------------*/

/*----------------------------------------------------------
    General Definitions
-----------------------------------------------------------*/

#define MV3_OK                      1
#define MV3_ERROR                   (-1)

#define MV3_TRUE                    1
#define MV3_FALSE                   0

#ifdef MV3_DEBUG_MSG
    #define DEBUG(_x_)            	mv3DebugOut _x_
#else
    #define DEBUG(_x_)
#endif


// Be careful - Inline option is different by compilers
// In case of C-compiler, use MakeFile with the option of -DINLINE=inline.
#ifndef INLINE
    #define INLINE                      __inline
#endif /* INLINE */

/*---------------------------------------------------------------------
	Image Buffer Address definitions	
----------------------------------------------------------------------*/
#define ADDR_ZERO                   0x00000000
#define ADDR_RAW_IMAGE_BUFFER       0x00000129


/*---------------------------------------------------------------------
	YUV411 Half size width
----------------------------------------------------------------------*/
// insert MV3019 by macprime 20050622
#ifdef FEATURE_MV3018
#define YUV411_HALF_SIZE_WIDTH      352
#elif defined(FEATURE_MV3019) 
#define YUV411_HALF_SIZE_WIDTH      1024
#endif


/*---------------------------------------------------------------------
	Delay definitions	
----------------------------------------------------------------------*/
#define DELAY_MPEGVIEW_END        1000
#define DELAY_I2C                 4000		/* 4ms */
#define DELAY_PREVIEW_STOP        3000		/* 3000ms */
#define DELAY_PREVIEW_PAUSE       DELAY_PREVIEW_STOP
#define DELAY_CAPTURE_END         DELAY_PREVIEW_STOP
#define DELAY_ENCODE_END          DELAY_PREVIEW_STOP
#define DELAY_SNAPSHOT_END        DELAY_PREVIEW_STOP
#define DELAY_INQUIRY             DELAY_PREVIEW_STOP
#define DELAY_BITBLT              DELAY_PREVIEW_STOP
#define DELAY_DECODE_END          DELAY_PREVIEW_STOP*2

/*---------------------------------------------------------------------
	Preview Step definitions
----------------------------------------------------------------------*/
#define MAX_ZOOM_STEP	        	128


/*---------------------------------------------------------------------
	I2C Number definitions
----------------------------------------------------------------------*/
#define MV3_DEFAULT_I2C_NUM         2


/*---------------------------------------------------------------------
	Macro definitions	
----------------------------------------------------------------------*/
#define mv3Delay1ms(x)              mv3Delay100us((x) * 10)

#define min(x1, x2)     ((x1 < x2) ? x1 : x2)
#define max(x1, x2)     ((x1 > x2) ? x1 : x2)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//                MACROS DEFINITIONS
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// write from MCU into MV301x
#define MV3_WRITE(addr, data)   outpw_(MV3_BASE_ADDR  + (addr), data);

// read from MV301x into MCU
#define	MV3_READ(addr)          inpw_ (MV3_BASE_ADDR  + (addr))


/*-----------------------------------------------------------------------
    Macro name      : convert888to565()
    Prototype       : convert888to565(mvUint32 pixel);
    Return          : 
    Argument        : pixel data
    Comments        : Transform RGB888 to RGB565
-----------------------------------------------------------------------*/
#define convert888to565(pixel)  (((pixel >> 8) & 0xF800) | ((pixel >> 5) & 0x07E0) | ((pixel >> 3) & 0x001F))


/*-----------------------------------------------------------------------
    Macro name      : convert666to565()
    Prototype       : convert666to565(mvUint32 pixel);
    Return          : 
    Argument        : pixel data
    Comments        : Transform RGB666 to RGB565
-----------------------------------------------------------------------*/
#define convert666to565(pixel)  (((pixel >> 2) & 0xF800) | ((pixel >> 1) & 0x07E0) | ((pixel >> 1) & 0x001F))


/*---------------------------------------------------------------------------
                           SHIFT_TEST

This macro returns a 1 (TRUE) if the specified bit in the mask is equal to 1,
else it returns a 0.

INPUT:
    val         = MASK
    shift       = Specified bit

NOTE: See SHIFT_FROM_MASK
---------------------------------------------------------------------------*/
#define SHIFT_TEST( val , shift ) ( (val) & (1U << (shift)) )
/*---------------------------------------------------------------------------
                           SHIFT_GET

This macro is used by SHIFT_FROM_MASK to test if the specified bit in the mask
is equal 1 or 0. If the specified bit equal to 1 then it returns # of shifts else
check the next bit.

INPUT:
    val         = MASK
    shift       = Current bit
    next_shift  = Next bit

NOTE: See SHIFT_FROM_MASK
----------------------------------------------------------------------------*/
#define SHIFT_GET( val, shift, next_shift) (SHIFT_TEST((val),(shift)) ?   \
(shift) : (next_shift))

/*----------------------------------------------------------------------------
                           SHIFT_FROM_MASK

This macro uses the register mask definitions in xxx.h to calculate
how many times a bit value needs to be shifted so that it modifies the correct
bit within a register.

MACRO FORMAT:
        if ((MASK && 0x01) != 0)
            return 0;                       //if 1st bit = 1 then do not shift
        else
            if ((MASK && 0x02) != 0)
                return 1;                   //if 2nd bit = 1 then shift once
            else
                if ((MASK && 0x04) != 0)
                    return 2;               //if 3rd bit = 1 then shift twice
                else
                    if ((MASK && 0x08) != 0)
                        return 3;           //if 4th bit = 1 then shift three
                    else
                            ......          //we check until bit 31
                            ......
                            ......


INPUT:
    x   = MASK

NOTE:   ONLY A MASK DEFINITION CAN BE USE AS AN INPUT FOR THIS MACRO!.
        THIS MACRO PRODUCES NO CODE, IT RETURNS A CONSTANT!.
---------------------------------------------------------------------------*/
#define SHIFT_FROM_MASK(x) (SHIFT_TEST(x##_MASK,0) ? 0 : \
                             (SHIFT_GET(x##_MASK,1, \
                              (SHIFT_GET(x##_MASK,2, \
                               (SHIFT_GET(x##_MASK,3, \
                                (SHIFT_GET(x##_MASK,4, \
                                 (SHIFT_GET(x##_MASK,5, \
                                  (SHIFT_GET(x##_MASK,6, \
                                   (SHIFT_GET(x##_MASK,7, \
                                    (SHIFT_GET(x##_MASK,8, \
                                     (SHIFT_GET(x##_MASK,9, \
                                      (SHIFT_GET(x##_MASK,10, \
                                       (SHIFT_GET(x##_MASK,11, \
                                        (SHIFT_GET(x##_MASK,12, \
                                         (SHIFT_GET(x##_MASK,13, \
                                          (SHIFT_GET(x##_MASK,14, \
                                           (SHIFT_GET(x##_MASK,15, \
                                            (SHIFT_GET(x##_MASK,16, \
                                             (SHIFT_GET(x##_MASK,17, \
                                              (SHIFT_GET(x##_MASK,18, \
                                               (SHIFT_GET(x##_MASK,19, \
                                                (SHIFT_GET(x##_MASK,20, \
                                                 (SHIFT_GET(x##_MASK,21, \
                                                  (SHIFT_GET(x##_MASK,22, \
                                                   (SHIFT_GET(x##_MASK,23, \
                                                    (SHIFT_GET(x##_MASK,24, \
                                                     (SHIFT_GET(x##_MASK,25, \
                                                      (SHIFT_GET(x##_MASK,26, \
                                                       (SHIFT_GET(x##_MASK,27, \
                                                        (SHIFT_GET(x##_MASK,28, \
                                                         (SHIFT_GET(x##_MASK,29, \
                                                          (SHIFT_GET(x##_MASK,30, \
                                                           (SHIFT_GET(x##_MASK,31,0) \
                           ) )))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

/*----------------------------------------------------------------------------
                           SHIFT_DN_FROM_MASK

This macro uses the register mask definitions in xxx.h to calculate
how many times a bit value needs to be shifted so that it modifies the correct
bit within a register.

The design is identical to SHIFT_FROM_MASK except that the full name of the
mask is used.  This is needed for use in existing that already take the full
mask name as a parameter.

INPUT:
    x   = MASK

NOTE:   ONLY A MASK DEFINITION CAN BE USE AS AN INPUT FOR THIS MACRO!.
        THIS MACRO PRODUCES NO CODE, IT RETURNS A CONSTANT!.
---------------------------------------------------------------------------*/
#define SHIFT_DN_FROM_MASK(x)     \
        ((x & 0x00000001) ? 0 :    \
         ((x & 0x00000002) ? 1 :    \
          ((x & 0x00000004) ? 2 :    \
           ((x & 0x00000008) ? 3 :    \
            ((x & 0x00000010) ? 4 :    \
             ((x & 0x00000020) ? 5 :    \
              ((x & 0x00000040) ? 6 :    \
               ((x & 0x00000080) ? 7 :    \
                ((x & 0x00000100) ? 8 :    \
                 ((x & 0x00000200) ? 9 :    \
                  ((x & 0x00000400) ? 10 :   \
                   ((x & 0x00000800) ? 11 :   \
                    ((x & 0x00001000) ? 12 :   \
                     ((x & 0x00002000) ? 13 :   \
                      ((x & 0x00004000) ? 14 :   \
                       ((x & 0x00008000) ? 15 :   \
                        ((x & 0x00010000) ? 16 :   \
                         ((x & 0x00020000) ? 17 :   \
                          ((x & 0x00040000) ? 18 :   \
                           ((x & 0x00080000) ? 19 :   \
                            ((x & 0x00100000) ? 20 :   \
                             ((x & 0x00200000) ? 21 :   \
                              ((x & 0x00400000) ? 22 :   \
                               ((x & 0x00800000) ? 23 :   \
                                ((x & 0x01000000) ? 24 :   \
                                 ((x & 0x02000000) ? 25 :   \
                                  ((x & 0x04000000) ? 26 :   \
                                   ((x & 0x08000000) ? 27 :   \
                                    ((x & 0x10000000) ? 28 :   \
                                     ((x & 0x20000000) ? 29 :   \
                                      ((x & 0x40000000) ? 30 :   \
                                       ((x & 0x80000000) ? 31 : 0 \
                                       ))))))))))))))))))))))))))))))))


