char BalMemoryPoolBuffer[0x200000];

void BalMemInit(void) {
	emo_printf("[Memory]: bpool()\n");
	bpool(&BalMemoryPoolBuffer, 0x200000);
}
