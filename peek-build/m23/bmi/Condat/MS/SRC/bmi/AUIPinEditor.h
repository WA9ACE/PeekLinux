#define E_PIN_INIT 1
#define E_PIN_UPDATE 2
#define E_PIN_DEINIT 3

/* CQ15542 - Definition added to enable completion of PUK processing in a single step */
#define E_PUK1_COMPLETE 4

/* Function prototypes */

T_MFW_HND	AUI_pin_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
void		AUI_pin_Destroy(T_MFW_HND win);
