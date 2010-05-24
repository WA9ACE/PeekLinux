#ifndef BAL_NM_HEADER
#define BAL_NM_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NO_SERVICE,     
    FULL_SERVICE,
    LIMITED_SERVICE,
    SEARCH_NETWORK
} reg_status_t;

void nm_registration (void);
void nm_deregistration (void);
void nm_deregistration_b (void);

reg_status_t nm_reg_status (void);

//Add Flight Mode by zhihui 08-10-27	  Begin
void nm_FlightMode_CreatTimer();
void nm_FlightMode_ClearTimer();
void nm_FlightMode_ResetTimer();
void	nm_FlightMode_StartTimer();
void nm_FlightMode_StopTimer();
void nm_FlightMode_TimerHandler();
void nm_FlightMode_state(int);
void nm_FlightMode_Set(int);
//Add Flight Mode by zhihui 08-10-27	  End

#ifdef __cplusplus
}
#endif

#endif
