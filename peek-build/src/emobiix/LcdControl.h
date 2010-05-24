#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_

#include "sysdefs.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
  LCD_MAIN,
  LCD_SUB,
  LCD_TOTAL
} DispLCDTypeT;

void BuiStatusSet(void);
uint32 LcdWakeUp(void);
extern void nm_deregistration_status(void);
extern void lcd_init_cmdsequence();
extern int power_on_flag;
extern int lcd_g_state;
extern int openlcdflag;

extern bool lowbatterypoweroff;

#ifdef __cplusplus
}
#endif
#endif /* _LCD_CTRL_H_ */
