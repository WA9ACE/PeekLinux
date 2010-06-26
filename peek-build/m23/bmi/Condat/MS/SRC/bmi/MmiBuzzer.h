/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBuzzer.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
                        
********************************************************************************

 $History: MmiBuzzer.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


#if !defined(BUZZER_H)
#define BUZZER_H

#define USE_PWT	// use build in tone generator cascade.


#define BUZZER_ASIC_CONF_REG		( *( ( volatile USHORT* ) 0xFFFEF008) )
#define	BUZZER_BZ_OR_PWT			0x20	// bit 5 = 0 for BZ =1 for PWT
#define BUZZER_FREE_BIT_CLOCK_EN			0xFF		// See HER207 Section 7.4.5
#define BUZZER_ARMIO_CNTL			( *( ( volatile USHORT* ) 0xFFFE4806) )

#define BUZZER_ON							0x01 		// Set bit 0
#define BUZZER_OFF							0xFE		// Reset bit 0

#ifdef USE_PWT

#define PWT_FRC_REG		( *( ( volatile UBYTE* ) 0xFFFE8800) )
#define PWT_VCR_REG		( *( ( volatile UBYTE* ) 0xFFFE8801) )
#define PWT_CGR_REG		( *( ( volatile UBYTE* ) 0xFFFE8802) )

#define BUZZER_MAX_VOL 0x3F


#else
// Bit definitions in CNTL
#define	BUZZER_SET_GPIO					0x0FDF		// Set bit 5
#define BUZZER_FREQ_CLOCK					0xC65D40 	// 13 MHz
#define BUZZER_2_POWER_9					0x0200		// See HER207 section 7.4.14


#define BUZZER_LEVEL_REG			( *( ( volatile USHORT* ) 0xFFFE4812) )
#define BUZZER_CNTL_REG				( *( ( volatile USHORT* ) 0xFFFE480E) )
#define BUZZER_LOAD_TIM_REG			( *( ( volatile USHORT* ) 0xFFFE4808) )
#endif




// define available buzzer tones
#if 0
#define g1  41
#define gs2 36
#define a1  33
#define as1 29
#define b1  25
#define c2  21
#define cs2 17
#define d2  13
#define ds2 7
#define e2  5
#define f2  48
#define fs2 44
#define fs1 45
#define g2 40
#define gs2 36
#define gs1 37
#define a2  32
#define as2 28
#define b2  24
#define c3  20
#define cs3 16
#define d3  12 
#define ds3 8
#define e3  4
#define f1  49
#define f2  48
#define f3  47  // 349
#define fs3 43  // 370
#define g3  39  // 392
#define gs3 35  // 415
#define a3  31  // 440
#define as3 27  // 466
#define b3  23  // 494
#define c4  19  // 523
#define cs4 15  // 554
#define d4  11  // 587
#define ds4  7  // 622
#define e4  3 // 659
#define f4  46  // 698
#define fs4  42  // 740
#define g4  38  // 784
#define gs4 34  // 831
#define a4  30  // 880
#define as4 26  // 932
#define b4  22  // 988
#define c5  18  // 1047
#define cs5 14  // 1109
#define d5  10  // 1175
#define ds5 6  // 1245
#define e5  2 // 1319
#define f5   45  // 1397
#define fs5  41  // 1480
#define g5  37  // 1568
#define gs5 33  // 1661
#define a5  29  // 1760
#define as5 25  // 1865
#define b5  21  // 1976
#define c6  17  // 2093
#define cs6 13  // 2217
#define d6   9  // 2349
#define ds6  5  // 2489
#define e6   1  // 2637
#define f6   44  // 2794
#define fs6  40  // 2960
#define g6  36  // 3136
#define gs6 32  // 3322
#define a6  28  // 3520
#define as6 24  // 3729
#define b6  20  // 3951
#define c7  16  // 4186
#define cs7 12  // 4435
#define d7  8  // 4699
#define ds7 4  // 4978
#define e7  0  // 5274
#endif
#define ds1  73
#define e1 72
#define f1  71
#define fs1 70
#define g1  69
#define gs1  68
#define a1 67
#define as1  66
#define b1 65
#define c2  64
#define cs2  63
#define d2 62
#define ds2 61
#define e2 60
#define f2 59
#define fs2 58
#define g2  57
#define gs2 56
#define a2  55
#define as2  54
#define b2 53
#define c3  52 
#define cs3 51
#define d3  50
#define ds3  49
#define e3  48
#define f3  47  // 349
#define fs3 46  // 370
#define g3  45  // 392
#define gs3 44  // 415
#define a3  43  // 440
#define as3 42  // 466
#define b3  41  // 494
#define c4  40  // 523
#define cs4 39  // 554
#define d4  38  // 587
#define ds4  37  // 622
#define e4  36 // 659
#define f4  35  // 698
#define fs4  34  // 740
#define g4  33  // 784
#define gs4 32  // 831
#define a4  31  // 880
#define as4 30  // 932
#define b4  29  // 988
#define c5  28  // 1047
#define cs5 27  // 1109
#define d5  26  // 1175
#define ds5 25  // 1245
#define e5  24 // 1319
#define f5   23  // 1397
#define fs5  22  // 1480
#define g5  21  // 1568
#define gs5 20  // 1661
#define a5  19  // 1760
#define as5 18  // 1865
#define b5  17  // 1976
#define c6  16  // 2093
#define cs6 15  // 2217
#define d6   14  // 2349
#define ds6  13  // 2489
#define e6   12  // 2637
#define f6   11  // 2794
#define fs6  10  // 2960
#define g6  9  // 3136
#define gs6 8  // 3322
#define a6  7  // 3520
#define as6 6  // 3729
#define b6  5  // 3951
#define c7  4  // 4186
#define cs7 3  // 4435
#define d7  2  // 4699
#define ds7 1  // 4978
#define e7  0  // 5274

#define bf1 as1
#define bf2 as2
#define bf3 as3
#define bf1 as1
#define BZ_REST 0x3F
#define BZ_TERM 0xFF




void buzzer_Initialize(void);
void buzzer_On(void);
void buzzer_Off(void);
void buzzer_Tone(int in_freqTone);
void buzzer_Volume(int in_Volume);


#endif
