#include "KeyMappings.h"
#include "Debug.h"

unsigned int MapKeyToInternal(unsigned int key)
{
	unsigned int mappedKey = 0;

	switch (key) {
		case KCD_0: mappedKey = '0'; break;
		case KCD_1: mappedKey = '1'; break;
		case KCD_2: mappedKey = '2'; break;
		case KCD_3: mappedKey = '3'; break;
		case KCD_4: mappedKey = '4'; break;
		case KCD_5: mappedKey = '5'; break;
		case KCD_6: mappedKey = '6'; break;
		case KCD_7: mappedKey = '7'; break;
		case KCD_8: mappedKey = '8'; break;
		case KCD_9: mappedKey = '9'; break;
		case KCD_A: mappedKey = 'a'; break;
		case KCD_B: mappedKey = 'b'; break;
		case KCD_C: mappedKey = 'c'; break;
		case KCD_D: mappedKey = 'd'; break;
		case KCD_E: mappedKey = 'e'; break;
		case KCD_F: mappedKey = 'f'; break;
		case KCD_G: mappedKey = 'g'; break;
		case KCD_H: mappedKey = 'h'; break;
		case KCD_I: mappedKey = 'i'; break;
		case KCD_J: mappedKey = 'j'; break;
		case KCD_K: mappedKey = 'k'; break;
		case KCD_L: mappedKey = 'l'; break;
		case KCD_M: mappedKey = 'm'; break;
		case KCD_N: mappedKey = 'n'; break;
		case KCD_O: mappedKey = 'o'; break;
		case KCD_P: mappedKey = 'p'; break;
		case KCD_Q: mappedKey = 'q'; break;
		case KCD_R: mappedKey = 'r'; break;
		case KCD_S: mappedKey = 's'; break;
		case KCD_T: mappedKey = 't'; break;
		case KCD_U: mappedKey = 'u'; break;
		case KCD_V: mappedKey = 'v'; break;
		case KCD_W: mappedKey = 'w'; break;
		case KCD_X: mappedKey = 'x'; break;
		case KCD_Y: mappedKey = 'y'; break;
		case KCD_Z: mappedKey = 'z'; break;
		case KCD_SPACE: mappedKey = ' '; break;
		case KCD_BACKSPACE: mappedKey = '\b'; break;
		case KCD_MNUSELECT: mappedKey = EKEY_ACTIVATE; break;
		case KCD_MNUUP: mappedKey = EKEY_FOCUSPREV; break;
		case KCD_MNUDOWN: mappedKey = EKEY_FOCUSNEXT; break;
		case KCD_LOCK: mappedKey = EKEY_ALTTAB; break;

		default:
			emo_printf("Unmapped key %d" NL, key);
			break;
	}

	emo_printf("Mapping key %d to %d", key, mappedKey);
	return mappedKey;
}
