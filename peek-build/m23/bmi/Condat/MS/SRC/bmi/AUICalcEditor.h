#define E_CALC_INIT 1
#define E_CALC_UPDATE 2
#define E_CALC_DEINIT 3

/* Function prototypes */

T_MFW_HND	AUI_calc_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
void		AUI_calc_Destroy(T_MFW_HND win);
