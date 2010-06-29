extern "C" int http_error_counter(void) {
  return 0;
}

extern "C" int onConnection(void) {
  return 0;
}

extern "C" int __callback_CIT_GPRS_Test(void) {
  return 0;
}

// Need to fix this
extern "C" int RestartDevice(void) {
	return 0;
}

extern "C" void BalTurnOnLight(void) {
	return;
}

extern "C" void ata_MCA_update_timezone(void) {


}
extern "C" void ata_get_aci_cid(void) {
}
extern "C" void ata_send_at(void) {
}
extern "C" void ata_sim_iccid(void) {
}
extern "C" void ata_sim_imsi(void) {
}
extern "C" void ata_sim_status(void) {
}
int ResourceDatabase;

void UIBitPutByte(unsigned char *blah, unsigned short foo, unsigned char moo, unsigned char poo) {

}
void UIBitGetByte(unsigned char *foo, unsigned short blah, unsigned char poo) {

}

class ResourceDatabaseC
{
public:
int GetResource(void **, void **);
};

int ResourceDatabaseC::GetResource(void **, void **) { 
	return 0;
} 

