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

