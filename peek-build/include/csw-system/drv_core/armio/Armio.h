/*
 * ARMIO.H  * * Control diagnostic bits * * Reference : GCS207 *
 */

#include "chipset.cfg"
#include "l1sw.cfg"

#include "sys_types.h"
#include "types.h"
#include "general.h"



#define ARMIO_IN        (MEM_ARMIO + 0x00) /* inputs */
#define ARMIO_OUT       (MEM_ARMIO + 0x02) /* outputs */
#define ARMIO_IO_CNTL   (MEM_ARMIO + 0x04) /* I/O control */
#define ARMIO_CNTL_REG  (MEM_ARMIO + 0x06) /* control ARMIO */
#define ARMIO_LOAD_TIM  (MEM_ARMIO + 0x08) /* load TIM */
#define MIR_1  *(volatile SYS_UWORD16 *) 0xFFFFFA0A  /*Mask interrupt handler*/

#if ((CHIPSET != 12) && (CHIPSET != 15))
  #define ARMIO_KBR_IN    (MEM_ARMIO + 0x0A) /* KBR inputs (rows) */
  #define ARMIO_KBR_OUT   (MEM_ARMIO + 0x0C) /* KBR outputs (columns) */
#endif

#define ARMIO_PWM_CNTL  (MEM_ARMIO + 0x0E) /* LIGHT/BUZZER control */
#define ARMIO_LIGHT     (MEM_ARMIO + 0x10) /* light value */
#define ARMIO_BUZZER    (MEM_ARMIO + 0x12) /* buzzer value */
#define ARMIO_CLOCKEN   0x0020

#if ((CHIPSET != 12) || (CHIPSET != 15))
  #define CLKM_IO_CNTL    MEM_IO_SEL         /* control IO */
#endif

#if ((CHIPSET == 12) || (CHIPSET == 15))
  #define GPIO_INTERRUPT_LEVEL_REG  * (volatile SYS_UWORD16 *) (MEM_ARMIO + 0x16)
  #define GPIO_INTERRUPT_MASK_REG   * (volatile SYS_UWORD16 *) (MEM_ARMIO + 0x18)
  #define GPIO_INTERRUPT_STATUS_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO + 0x1E)
  #define GPIO_SOFT_CLEAR_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO + 0x22)
  #define GPIO_JOGDIAL_MODE_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO+0x14)
  #define GPIO_JD_DEBOUNCING_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO + 0x1A)

#else
  #define ARMIO_GPIO_EVENT_MODE (MEM_ARMIO + 0x14) /* GPIO event mode */
  #define ARMIO_KBD_GPIO_INT    (MEM_ARMIO + 0x16) /* Kbd/GPIO IRQ register */
  #define ARMIO_KBD_GPIO_MASKIT (MEM_ARMIO + 0x18) /* Kbd/GPIO mask IRQ */
#endif

#if (CHIPSET == 15)
  /* register set for GPIO1 block */
  #define ARMIO1_IN        (MEM_ARMIO1 + 0x00) /* inputs */
  #define ARMIO1_OUT       (MEM_ARMIO1 + 0x02) /* outputs */
  #define ARMIO1_IO_CNTL   (MEM_ARMIO1 + 0x04) /* I/O control */
  #define ARMIO1_CNTL_REG  (MEM_ARMIO1 + 0x06) /* control ARMIO */
  #define ARMIO1_LOAD_TIM  (MEM_ARMIO1 + 0x08) /* load TIM */
  #define ARMIO1_PWM_CNTL  (MEM_ARMIO1 + 0x0E) /* LIGHT/BUZZER control */
  #define ARMIO1_LIGHT     (MEM_ARMIO1 + 0x10) /* light value */
  #define ARMIO1_BUZZER    (MEM_ARMIO1 + 0x12) /* buzzer value */
  #define GPIO1_INTERRUPT_LEVEL_REG  * (volatile SYS_UWORD16 *) (MEM_ARMIO1 + 0x16)
  #define GPIO1_INTERRUPT_MASK_REG   * (volatile SYS_UWORD16 *) (MEM_ARMIO1 + 0x18)
  #define GPIO1_INTERRUPT_STATUS_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO1 + 0x1E)
  #define GPIO1_SOFT_CLEAR_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO1 + 0x22)
  #define GPIO1_JOGDIAL_MODE_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO1+0x14)
  #define GPIO1_JD_DEBOUNCING_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO1 + 0x1A)

  /* register set for GPIO2 block */
  #define ARMIO2_IN        (MEM_ARMIO2 + 0x00) /* inputs */
  #define ARMIO2_OUT       (MEM_ARMIO2 + 0x02) /* outputs */
  #define ARMIO2_IO_CNTL   (MEM_ARMIO2 + 0x04) /* I/O control */
  #define ARMIO2_CNTL_REG  (MEM_ARMIO2 + 0x06) /* control ARMIO */
  #define ARMIO2_LOAD_TIM  (MEM_ARMIO2 + 0x08) /* load TIM */
  #define ARMIO2_PWM_CNTL  (MEM_ARMIO2 + 0x0E) /* LIGHT/BUZZER control */
  #define ARMIO2_LIGHT     (MEM_ARMIO2 + 0x10) /* light value */
  #define ARMIO2_BUZZER    (MEM_ARMIO2 + 0x12) /* buzzer value */
  #define GPIO2_INTERRUPT_LEVEL_REG  * (volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x16)
  #define GPIO2_INTERRUPT_MASK_REG   * (volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x18)
  #define GPIO2_INTERRUPT_STATUS_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x1E)
  #define GPIO2_SOFT_CLEAR_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x22)
  #define GPIO2_JOGDIAL_MODE_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x14)
  #define GPIO2_JD_DEBOUNCING_REG *(volatile SYS_UWORD16 *) (MEM_ARMIO2 + 0x1A)
#endif

#if ((CHIPSET != 12) && (CHIPSET != 15))
  #define ARMIO_DCD (2) /* IO used for DCD on C-Sample - Output */
  #define ARMIO_DTR (3) /* IO used for DTR on C-Sample - Input */
#endif

#define CONFIG_ALL_AS_INPUT (0xFFFF)
#define ARMIO_FALLING_EDGE (0)
#define ARMIO_RISING_EDGE  (1)
#define MASK_ALL_INTERRUPT (0xFFFF)
#define GPIO_INT_ENABLED (1)
#define GPIO_INT_DISABLED (0)
#define DEBOUNCE_EDGE (5)
#define DEBOUNCE_13MHZ_CLOCK (6)

#define ARMIO_MIR_1_MASK (0xFF9E)

#if ((CHIPSET != 12) && (CHIPSET != 15))
  #define ARMIO_KEYPDAD_INT (0x0001)
  #define ARMIO_GPIO_INT    (0x0002)

  #define ARMIO_MASKIT_KBD  (0x0001)
  #define ARMIO_MASKIT_GPIO (0x0002)

  void AI_EnableBit(int bit);
  void AI_DisableBit(int bit);
#endif
UINT8 AI_SetBit(int bit);
UINT8 AI_ResetBit(int bit);
UINT8 AI_ConfigBitAsOutput(int bit);
UINT8 AI_ConfigBitAsInput(int bit);
SYS_BOOL AI_ReadBit(int bit);

#if (OP_L1_STANDALONE == 0)
  void AI_Power(SYS_UWORD8 power);
#endif

void AI_ResetTspIO(void);
void AI_ResetDbgReg(void);
void AI_ResetIoConfig(void);
void AI_InitIOConfig(void);
void AI_ClockEnable (void);

void AI_SelectIOForIT (SYS_UWORD16 Pin, SYS_UWORD16 Edge);
#if ((CHIPSET != 12) && (CHIPSET != 15))
  int  AI_CheckITSource (SYS_UWORD16 Source);
#endif

UINT8 AI_MaskIT (SYS_UWORD16 Source);
UINT8 AI_UnmaskIT (SYS_UWORD16 Source);
UINT8 AI_Enable_Debounce(UINT8 gpio_pin, UINT8 gpio_edge, UINT8 gpio_clock);
void gpio_sleep(void);
void gpio_wakeup(void);
void AI_Set_Clear_Reg(SYS_UWORD8 GPIO_int_pin);
void Al_int_handler(void);
void AI_Enable_int(SYS_UWORD8 module_no);
UINT8 Al_int_config(SYS_UWORD8 pin, void(*callback_func_ptr)(void) ,SYS_UWORD16 Edge);
void AI_int_init();
UINT8 AI_Disable_Debounce(SYS_UWORD8 pin);
void AI_Set_Debounce_Time(UINT16 Time);
