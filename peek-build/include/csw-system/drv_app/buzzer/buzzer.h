/*
 * BUZZER.H
 *
 * Buzzer and light control
 *
 * Target : ARM 
 *
 * Copyright (c) Texas Instruments 1996
 *
 */

#define BZ_LEVEL       ARMIO_BUZZER
#define LT_LEVEL       ARMIO_LIGHT

// Bit definitions in CNTL
#define BZ_ON			0x01	// start buzzer
#define LT_ON			0x02	// start light

// Buzzer control
void BZ_Init(void);
void BZ_Enable(void);
void BZ_Disable(void);
void BZ_Tone(int f);
void BZ_Volume(int v);
void BZ_IntHandler(void);
void BZ_KeyBeep_ON(void);
void BZ_KeyBeep_OFF(void);

// LCD and keypad Light control
void LT_Enable(void);
void LT_Disable(void);
void LT_Level(SYS_WORD8 level);
SYS_BOOL LT_Status(void);
