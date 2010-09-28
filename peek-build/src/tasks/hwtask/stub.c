int BalDispInitFontInfo(void) {return 0;}
int BalMemoryPoolBufferGet(void) {return 0;}
int BalMemoryPoolSizeGet(void) {return 0;}
BalPswProtocolStackEnable(void) {}
BalTurnOnLight(void) { }
HwdPwrOffBoard(void) { }  
SysTime2Secs(void) {}       
SysTimeDataGet(void) { }     

extern void *malloc(unsigned int size);

void *bget(long size) { return malloc(size); }
int bpool(void) {return 0; }            

void brel(void *ptr) {free(ptr); return; }                      

int bstats(void) {return 0;}            
void MonPrintf(void) {}

typedef struct {
        unsigned int eSndAndAlert;
        unsigned int eBKProfile;
        unsigned int eDispTheme;
        unsigned int eFlightMode;
}SettingModeHelplerInfo;

SettingModeHelplerInfo stCurSettingMode = {0,0,0,0};

unsigned int timer_state = 0;
unsigned int http_error_counter = 0;

int fResumeSettingFromConfigFile(void) {


}

/* cant see where they used in bal_socket.o so we might have to investigate */
void ata_nm_set_gprs_reg_status(void) {

}

void ata_nm_update_rssi(void) {

}

void nm_plmn_get_running(void) {


}

void bmi_version(void) {

}

void mfwExtIn(void) {

}

void mfw_version(void) {

}

void rAT_PercentCPNUMS(void) {

}

void rAT_PercentCPROAM(void) {

}

void rAT_PercentCPVWI(void) {

}
