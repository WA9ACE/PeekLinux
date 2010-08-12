#include "KeyMappings.h"
#include "Debug.h"

int MapKeyToInternal(int key)
{
	int mappedKey = 0;

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
	/*
		 case KCD_a: mappedKey = 'a'; break;
		 case KCD_b: mappedKey = 'b'; break;
		 case KCD_c: mappedKey = 'c'; break;
		 case KCD_d: mappedKey = 'd'; break;
		 case KCD_e: mappedKey = 'e'; break;
		 case KCD_f: mappedKey = 'f'; break;
		 case KCD_g: mappedKey = 'g'; break;
		 case KCD_h: mappedKey = 'h'; break;
		 case KCD_i: mappedKey = 'i'; break;
		 case KCD_j: mappedKey = 'j'; break;
		 case KCD_k: mappedKey = 'k'; break;
		 case KCD_l: mappedKey = 'l'; break;
		 case KCD_m: mappedKey = 'm'; break;
		 case KCD_n: mappedKey = 'n'; break;
		 case KCD_o: mappedKey = 'o'; break;
		 case KCD_p: mappedKey = 'p'; break;
		 case KCD_q: mappedKey = 'q'; break;
		 case KCD_r: mappedKey = 'r'; break;
		 case KCD_s: mappedKey = 's'; break;
		 case KCD_t: mappedKey = 't'; break;
		 case KCD_u: mappedKey = 'u'; break;
		 case KCD_v: mappedKey = 'v'; break;
		 case KCD_w: mappedKey = 'w'; break;
		 case KCD_x: mappedKey = 'x'; break;
		 case KCD_y: mappedKey = 'y'; break;
		 case KCD_z: mappedKey = 'z'; break;
	 */
		case KCD_A: mappedKey = 'A'; break;
		case KCD_B: mappedKey = 'B'; break;
		case KCD_C: mappedKey = 'C'; break;
		case KCD_D: mappedKey = 'D'; break;
		case KCD_E: mappedKey = 'E'; break;
		case KCD_F: mappedKey = 'F'; break;
		case KCD_G: mappedKey = 'G'; break;
		case KCD_H: mappedKey = 'H'; break;
		case KCD_I: mappedKey = 'I'; break;
		case KCD_J: mappedKey = 'J'; break;
		case KCD_K: mappedKey = 'K'; break;
		case KCD_L: mappedKey = 'L'; break;
		case KCD_M: mappedKey = 'M'; break;
		case KCD_N: mappedKey = 'N'; break;
		case KCD_O: mappedKey = 'O'; break;
		case KCD_P: mappedKey = 'P'; break;
		case KCD_Q: mappedKey = 'Q'; break;
		case KCD_R: mappedKey = 'R'; break;
		case KCD_S: mappedKey = 'S'; break;
		case KCD_T: mappedKey = 'T'; break;
		case KCD_U: mappedKey = 'U'; break;
		case KCD_V: mappedKey = 'V'; break;
		case KCD_W: mappedKey = 'W'; break;
		case KCD_X: mappedKey = 'X'; break;
		case KCD_Y: mappedKey = 'Y'; break;
		case KCD_Z: mappedKey = 'Z'; break;
		case KCD_SPACE: mappedKey = ' '; break;
		case KCD_BACKSPACE: mappedKey = '\b'; break;
		case KCD_MNUSELECT:
		case KCD_MNUUP:
		case KCD_MNUDOWN:
			mappedKey = key;
			break;
		default:
			emo_printf("Unmapped key %d" NL, key);
			break;
	}

	return mappedKey;
}
