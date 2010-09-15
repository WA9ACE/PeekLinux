#include "KeyMappings.h"
#include "Debug.h"

unsigned int MapKeyToInternal(unsigned int key)
{
	unsigned int mappedKey = 0;

	switch (key) {
		case SYS_0_KEY: mappedKey = '0'; break;
		case SYS_1_KEY: mappedKey = '1'; break;
		case SYS_2_KEY: mappedKey = '2'; break;
		case SYS_3_KEY: mappedKey = '3'; break;
		case SYS_4_KEY: mappedKey = '4'; break;
		case SYS_5_KEY: mappedKey = '5'; break;
		case SYS_6_KEY: mappedKey = '6'; break;
		case SYS_7_KEY: mappedKey = '7'; break;
		case SYS_8_KEY: mappedKey = '8'; break;
		case SYS_9_KEY: mappedKey = '9'; break;
		case SYS_a_KEY: mappedKey = 'a'; break;
		case SYS_b_KEY: mappedKey = 'b'; break;
		case SYS_c_KEY: mappedKey = 'c'; break;
		case SYS_d_KEY: mappedKey = 'd'; break;
		case SYS_e_KEY: mappedKey = 'e'; break;
		case SYS_f_KEY: mappedKey = 'f'; break;
		case SYS_g_KEY: mappedKey = 'g'; break;
		case SYS_h_KEY: mappedKey = 'h'; break;
		case SYS_i_KEY: mappedKey = 'i'; break;
		case SYS_j_KEY: mappedKey = 'j'; break;
		case SYS_k_KEY: mappedKey = 'k'; break;
		case SYS_l_KEY: mappedKey = 'l'; break;
		case SYS_m_KEY: mappedKey = 'm'; break;
		case SYS_n_KEY: mappedKey = 'n'; break;
		case SYS_o_KEY: mappedKey = 'o'; break;
		case SYS_p_KEY: mappedKey = 'p'; break;
		case SYS_q_KEY: mappedKey = 'q'; break;
		case SYS_r_KEY: mappedKey = 'r'; break;
		case SYS_s_KEY: mappedKey = 's'; break;
		case SYS_t_KEY: mappedKey = 't'; break;
		case SYS_u_KEY: mappedKey = 'u'; break;
		case SYS_v_KEY: mappedKey = 'v'; break;
		case SYS_w_KEY: mappedKey = 'w'; break;
		case SYS_x_KEY: mappedKey = 'x'; break;
		case SYS_y_KEY: mappedKey = 'y'; break;
		case SYS_z_KEY: mappedKey = 'z'; break;
        case SYS_A_KEY: mappedKey = 'A'; break;
        case SYS_B_KEY: mappedKey = 'B'; break;
        case SYS_C_KEY: mappedKey = 'C'; break;
        case SYS_D_KEY: mappedKey = 'D'; break;
        case SYS_E_KEY: mappedKey = 'E'; break;
        case SYS_F_KEY: mappedKey = 'F'; break;
        case SYS_G_KEY: mappedKey = 'G'; break;
        case SYS_H_KEY: mappedKey = 'H'; break;
        case SYS_I_KEY: mappedKey = 'I'; break;
        case SYS_J_KEY: mappedKey = 'J'; break;
        case SYS_K_KEY: mappedKey = 'K'; break;
        case SYS_L_KEY: mappedKey = 'L'; break;
        case SYS_M_KEY: mappedKey = 'M'; break;
        case SYS_N_KEY: mappedKey = 'N'; break;
        case SYS_O_KEY: mappedKey = 'O'; break;
        case SYS_P_KEY: mappedKey = 'P'; break;
        case SYS_Q_KEY: mappedKey = 'Q'; break;
        case SYS_R_KEY: mappedKey = 'R'; break;
        case SYS_S_KEY: mappedKey = 'S'; break;
        case SYS_T_KEY: mappedKey = 'T'; break;
        case SYS_U_KEY: mappedKey = 'U'; break;
        case SYS_V_KEY: mappedKey = 'V'; break;
        case SYS_W_KEY: mappedKey = 'W'; break;
        case SYS_X_KEY: mappedKey = 'X'; break;
        case SYS_Y_KEY: mappedKey = 'Y'; break;
        case SYS_Z_KEY: mappedKey = 'Z'; break;
		/* Special Chars */
		case SYS_EXCAL_KEY: mappedKey = '!'; break;
		case SYS_QUESTION_KEY: mappedKey = '?'; break;
		case SYS_POUND_KEY: mappedKey = '#'; break;
		case SYS_DOLLAR_KEY: mappedKey = '$'; break;
		case SYS_STAR_KEY: mappedKey = '*'; break;
		case SYS_LEFT_BRACKET_KEY: mappedKey = '('; break;
		case SYS_RIGHT_BRACKET_KEY: mappedKey = ')'; break;
		case SYS_DOT: mappedKey = '.'; break;
		case SYS_AT: mappedKey = '@'; break;
		case SYS_QUOTE: mappedKey = '\''; break;
		case SYS_REDUCE: mappedKey = '-'; break;
		case SYS_COMMA: mappedKey = ','; break;
		case SYS_PERCENT_KEY: mappedKey = '%'; break;
		case SYS_SLASH_KEY: mappedKey = '/'; break;
		case SYS_AND_KEY: mappedKey = '&'; break;
		case SYS_SPACE: mappedKey = ' '; break;
		case SYS_COLON_KEY: mappedKey = ':'; break;
		case SYS_SEMICOLON_KEY: mappedKey = ';'; break;
		case SYS_DOUBLE_QUESTIONMASKS_KEY: mappedKey = '"'; break;
		case SYS_UNDERLINE_KEY: mappedKey = '_'; break;
		case SYS_EQUAL_KEY: mappedKey = '='; break;
		case SYS_BACKSPACE: mappedKey = '\b'; break;
		case SYS_ENTER: /* Map to down click for now */
		case SYS_WHEEL: mappedKey = EKEY_ACTIVATE; break;
		case SYS_WHEEL_BACK: mappedKey = EKEY_FOCUSPREV; break;
		case SYS_WHEEL_FORWARD: mappedKey = EKEY_FOCUSNEXT; break;
		case SYS_CANCEL_KEY: /* Map to App switch for now */
		case SYS_LOCK: mappedKey = EKEY_ALTTAB; break;

		default:
			emo_printf("Unmapped key %d" NL, key);
			break;
	}

	emo_printf("Mapping key %d to %d", key, mappedKey);
	return mappedKey;
}
