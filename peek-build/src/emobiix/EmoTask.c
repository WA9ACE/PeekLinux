void EmoTask(void) {
	BalMemInit();
	EmoStatusSet();
	while(1) {
		TCCE_Task_Sleep(50);
	}
}
