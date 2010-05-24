#ifndef _SCROLL_WHEEL_MCU_
#define _SCROLL_WHEEL_MCU_

typedef enum {
    SCR_DOWN,
    SCR_UP
} scr_direct;

typedef enum {
    SCR_FALSE,
    SCR_TRUE
} scr_result;

typedef void (*scroll_wheel_cb)(scr_direct direct, unsigned char step);

extern scr_result init_scroll_wheel(scroll_wheel_cb cb);


#endif // _SCROLL_WHEEL_MCU_
