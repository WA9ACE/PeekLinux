/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   PhoneBook
 $File:       MmiMelody_Data.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

********************************************************************************

 $History: MmiMelody_data.c

  25/10/00      Original Condat(UK) BMI version.

 $End

*******************************************************************************/

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "MmiBuzzer.h"

typedef struct
{
  USHORT      command_1;
  USHORT      command_2;
  USHORT      length;
} T_DESCR;

#define DEF_MELODY(name) const T_DESCR name[] = {

#define END_MELODY BZ_TERM,0 };

//#include "tb1.hm"

#ifdef USE_PWT
#else

const UINT16 MELODY_BUZ_FREQ[] =  {
  0, // REST
//    A    A#  B  C  C#  D   D#  E    F   F#  G   G#
    27,  29,  31,  33,  35,  37,  39,  41,  44,  46,  49,  52,
    55,  58,  62,  65,  69,  73,  78,  82,  87,  92,  98,  104,
   110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208,
   220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415,
   440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831,  // MIDDLE
   880, 932, 988,1047,1109,1175,1245,1319,1397,1480,1568,1661,
  1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,
  3520,3729,3951,4186,4435,4699,4978,5274,5588,5920,6272,6645,
  7040,7459,7902,8372,8870,9397,9956,10548,11175,11840,12544,13290
};

#endif

/*
  abc tune notation
  =================

The following letters are used to represent notes:-


                                                      d'
                                                -c'- ----
                                             b
                                        -a- --- ---- ----
                                       g
 ------------------------------------f-------------------
                                   e
 --------------------------------d-----------------------
                               c
 ----------------------------B---------------------------
                           A
 ------------------------G-------------------------------
                       F
 --------------------E-----------------------------------
                   D
 ---- ---- ---- -C-
            B,
 ---- -A,-
  G,
*/

#define TDMA_12 55

DEF_MELODY(rising_chromatic)
f1, 0,TDMA_12,
fs1, 0,TDMA_12,
g1, 0,TDMA_12,
gs1, 0,TDMA_12,
a1, 0,TDMA_12,
as1, 0,TDMA_12,
b1, 0,TDMA_12,
c2, 0,TDMA_12,
cs2, 0,TDMA_12,
d2, 0,TDMA_12,
ds2, 0,TDMA_12,
e2, 0,TDMA_12,
f2, 0,TDMA_12,
fs2, 0,TDMA_12,
g2, 0,TDMA_12,
gs2, 0,TDMA_12,
a2, 0,TDMA_12,
as2, 0,TDMA_12,
b2, 0,TDMA_12,
c3, 0,TDMA_12,
cs3, 0,TDMA_12,
d3, 0,TDMA_12,
ds3, 0,TDMA_12,
e3, 0,TDMA_12,
f3, 0,TDMA_12,
fs3, 0,TDMA_12,
g3, 0,TDMA_12,
gs3, 0,TDMA_12,
a3, 0,TDMA_12,
as3, 0,TDMA_12,
b3, 0,TDMA_12,
c4, 0,TDMA_12,
cs4, 0,TDMA_12,
d4, 0,TDMA_12,
ds4, 0,TDMA_12,
e4, 0,TDMA_12,
f4, 0,TDMA_12,
fs4, 0,TDMA_12,
g4, 0,TDMA_12,
gs4, 0,TDMA_12,
a4, 0,TDMA_12,
as4, 0,TDMA_12,
b4, 0,TDMA_12,
c5, 0,TDMA_12,
cs5, 0,TDMA_12,
d5, 0,TDMA_12,
ds5, 0,TDMA_12,
e5, 0,TDMA_12,
END_MELODY

DEF_MELODY(rising_cscale)
f1, 0,TDMA_12,
g1, 0,TDMA_12,
a1, 0,TDMA_12,
b1, 0,TDMA_12,
c2, 0,TDMA_12,
d2, 0,TDMA_12,
e2, 0,TDMA_12,
f2, 0,TDMA_12,
g2, 0,TDMA_12,
a2, 0,TDMA_12,
b2, 0,TDMA_12,
c3, 0,TDMA_12,
d3, 0,TDMA_12,
e3, 0,TDMA_12,
f3, 0,TDMA_12,
g3, 0,TDMA_12,
a3, 0,TDMA_12,
b3, 0,TDMA_12,
c4, 0,TDMA_12,
d4, 0,TDMA_12,
e4, 0,TDMA_12,
f4, 0,TDMA_12,
g4, 0,TDMA_12,
a4, 0,TDMA_12,
b4, 0,TDMA_12,
c5, 0,TDMA_12,
d5, 0,TDMA_12,
e5, 0,TDMA_12,
END_MELODY

// Coca cola
DEF_MELODY(MELODY_ONE)
 fs3,0,TDMA_12,
 fs3,0,TDMA_12,
 fs3,0,TDMA_12,
 fs3,0,TDMA_12,
 g3,0,TDMA_12,
 fs3,0,TDMA_12,
 e3,0,TDMA_12,
 e3,0,TDMA_12,
 a3,0,TDMA_12,
 fs3,0,TDMA_12,
 d3,0,TDMA_12,
 0xFFFF,0xFFFF,0,
END_MELODY

DEF_MELODY(The_Ale_is_dear)
fs4,30,TDMA_12,        e4,30,TDMA_12, fs4,30,TDMA_12, b3,30,TDMA_12,        fs4,30,TDMA_12, e4,30,TDMA_12,
fs4,30,TDMA_12, gs4,30,TDMA_12, e4,30,TDMA_12, fs4,30,TDMA_12, cs4,30,TDMA_12, a4,30,TDMA_12, cs4,30,TDMA_12, e4,30,TDMA_12,
fs4,60,TDMA_12,        e4,30,TDMA_12, fs4,30,TDMA_12, b3,60,TDMA_12,        cs4,30,TDMA_12, e4,30,TDMA_12,
fs4,30,TDMA_12, e4,30,TDMA_12, d4,30,TDMA_12, cs4,30,TDMA_12, b3,30,TDMA_12,  cs3,30,TDMA_12, d3,30,TDMA_12, cs3,30,TDMA_12,

fs4,60,TDMA_12,        e4,30,TDMA_12, fs4,30,TDMA_12, b3,60,TDMA_12,        fs4,30,TDMA_12, e4,30,TDMA_12,
fs4,30,TDMA_12, g4,30,TDMA_12, e4,30,TDMA_12, fs4,30,TDMA_12, cs4,30,TDMA_12, a4,30,TDMA_12, cs4,30,TDMA_12, e4,30,TDMA_12,
fs4,60,TDMA_12,        e4,30,TDMA_12, fs4,30,TDMA_12, b3,60,TDMA_12,        cs4,30,TDMA_12, e4,30,TDMA_12,
fs4,30,TDMA_12, e4,30,TDMA_12, d4,30,TDMA_12, cs4,30,TDMA_12, b3,30,TDMA_12,  cs3,30,TDMA_12, d3,30,TDMA_12, cs3,30,TDMA_12,

b3,30,TDMA_12, b3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, d4,60,TDMA_12,        cs4,30,TDMA_12, b3,30,TDMA_12,
a3,30,TDMA_12, a3,30,TDMA_12, a3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, a3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12,
b3,30,TDMA_12, b3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, d4,60,TDMA_12,        cs4,30,TDMA_12, b3,30,TDMA_12,
fs4,60,TDMA_12,        e4,30,TDMA_12, cs4,30,TDMA_12, b3,60,TDMA_12,        b3,60,TDMA_12,

b3,30,TDMA_12, b3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, d4,60,TDMA_12,        cs4,30,TDMA_12, b3,30,TDMA_12,
a3,30,TDMA_12, a3,30,TDMA_12, a3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, a3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12,
b3,30,TDMA_12, b3,30,TDMA_12, b3,30,TDMA_12, cs4,30,TDMA_12, d4,60,TDMA_12,        cs4,30,TDMA_12, b3,30,TDMA_12,
fs4,60,TDMA_12,        e4,30,TDMA_12, cs4,30,TDMA_12, b3,60,TDMA_12,        b3,60,TDMA_12,
END_MELODY

DEF_MELODY(Danza_de_Astureses)
c4,60,TDMA_12, d4,60,TDMA_12,
e4,60,TDMA_12, d4,24,TDMA_12,
e4,24,TDMA_12, f4,24,TDMA_12,
e4,24,TDMA_12, d4,24,TDMA_12,
c4,24,TDMA_12, b3,60,TDMA_12,
g3,60,TDMA_12, c4,60,TDMA_12,
d4,60,TDMA_12, e4,60,TDMA_12,
d4,24,TDMA_12, e4,24,TDMA_12,
fs4,24,TDMA_12, d4,24,TDMA_12,
e4,24,TDMA_12, f4,24,TDMA_12,
g4,120,TDMA_12,
END_MELODY


DEF_MELODY(MELODY_HELLO)
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
  e3,0,TDMA_12,
   0xFFFF,       0xFFFF,        0,
END_MELODY

DEF_MELODY(bugle)
c4, 80,TDMA_12,
BZ_REST, 5,TDMA_12,
e4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
g3, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 80,TDMA_12,
BZ_REST, 45,TDMA_12,
c4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
e4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
g3, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 120,TDMA_12,
BZ_REST, 45,TDMA_12,
c4, 80,TDMA_12,
BZ_REST, 5,TDMA_12,
e4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
g3, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 80,TDMA_12,
BZ_REST, 45,TDMA_12,
c4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 60,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 20,TDMA_12,
BZ_REST, 5,TDMA_12,
e4, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
g3, 40,TDMA_12,
BZ_REST, 5,TDMA_12,
c4, 120,TDMA_12,
BZ_REST, 40,TDMA_12,
END_MELODY

