#ifndef __PEEK_KMAP_H
#define __PEEK_KMAP_H


typedef enum
{
    SYS_KEY_ID_START  = 0,          /* do not modify it */
    SYS_SHIFT ,                     /*do not modify this position,zhangfanghui*/
    SYS_0_KEY,
    SYS_1_KEY,              /* '1' key */
    SYS_2_KEY,              /* '2' key */
    SYS_3_KEY,              /* '3' key */
    SYS_4_KEY,              /* '4' key */
    SYS_5_KEY,              /* '5' key */
    SYS_6_KEY,              /* '6' key */
    SYS_7_KEY,              /* '7' key */
    SYS_8_KEY,              /* '8' key */
    SYS_9_KEY,              /* '9' key */

    SYS_a_KEY,
    SYS_b_KEY,
    SYS_c_KEY,
    SYS_d_KEY,
    SYS_e_KEY,
    SYS_f_KEY,
    SYS_g_KEY,
    SYS_h_KEY,
    SYS_i_KEY,
    SYS_j_KEY,
    SYS_k_KEY,
    SYS_l_KEY,
    SYS_m_KEY,
    SYS_n_KEY,
    SYS_o_KEY,
    SYS_p_KEY,
    SYS_q_KEY,
    SYS_r_KEY,
    SYS_s_KEY,
    SYS_t_KEY,
    SYS_u_KEY,
    SYS_v_KEY,
    SYS_w_KEY,
    SYS_x_KEY,
    SYS_y_KEY,
    SYS_z_KEY,


    SYS_ENTER,
    SYS_DOT,     // .
    SYS_AT,      // @
    SYS_QUOTE,    // '
    SYS_LOCK,
    SYS_REDUCE,//-
    SYS_SPACE,
    SYS_COMMA,   // ,
    SYS_WHEEL,
    SYS_CANCEL_KEY,
    SYS_BACKSPACE,
    SYS_PWR_KEY,            /* Power key */

    SYS_A_KEY,
    SYS_B_KEY,
    SYS_C_KEY,
    SYS_D_KEY,
    SYS_E_KEY,
    SYS_F_KEY,
    SYS_G_KEY,
    SYS_H_KEY,
    SYS_I_KEY,
    SYS_J_KEY,
    SYS_K_KEY,
    SYS_L_KEY,
    SYS_M_KEY,
    SYS_N_KEY,
    SYS_O_KEY,
    SYS_P_KEY,
    SYS_Q_KEY,
    SYS_R_KEY,
    SYS_S_KEY,
    SYS_T_KEY,
    SYS_U_KEY,
    SYS_V_KEY,
    SYS_W_KEY,
    SYS_X_KEY,
    SYS_Y_KEY,
    SYS_Z_KEY,

    SYS_EXCAL_KEY,            //excalmatory point,!
    SYS_QUESTION_KEY,         //question mark,?
    SYS_POUND_KEY,          // '#' key
    SYS_DOLLAR_KEY,         //$ key
    SYS_PERCENT_KEY,            //% key
    SYS_SLASH_KEY,          // '/' key
    SYS_AND_KEY,            // & key
    SYS_STAR_KEY,           // '*' key
    SYS_LEFT_BRACKET_KEY,   //( key
    SYS_RIGHT_BRACKET_KEY,  //) key

    SYS_WHEEL_FORWARD,
    SYS_WHEEL_BACK,


    SYS_COLON_KEY,          // : key
    SYS_SEMICOLON_KEY,      //; key
    SYS_DOUBLE_QUESTIONMASKS_KEY, // " key
    SYS_UNDERLINE_KEY,          // _ key
    SYS_EQUAL_KEY,              //= key
    SYS_WHEEL_FORWARD_SHIFT,     //shift + wheel forward
    SYS_WHEEL_BACK_SHIFT,       //shift + wheel back
    SYS_WHEEL_SHIFT,            //shift +wheel press
    SYS_RIGHT_KEY,          /* Right key */
    SYS_LEFT_KEY,           /* Left key  */
    SYS_UP_KEY,             /* Up key */
    SYS_DOWN_KEY,           /* Down key */

    SYS_SEND_KEY,           /* Send key */
    SYS_END_KEY,            /* End key */

    SYS_CLR_KEY,            /* Clear key */
    SYS_MEM_KEY,
    SYS_FUNCTION_KEY,       /* Function key */

    SYS_SELECT_KEY,         /* Select key */
    SYS_CAMERA_KEY,         /* Camera key */

    SYS_RIGHT_SOFT_KEY,     /* right soft key */
    SYS_LEFT_SOFT_KEY,      /* left soft key */

    SYS_VOLUME_UP_KEY,      /* volume up key */
    SYS_VOLUME_DOWN_KEY,    /* volume down key */

    SYS_HOME_KEY,           /* added by neil */
    SYS_BACK_KEY,

    SYS_EAR_KEY,            /* ear key */

    SYS_SMS_KEY,            /* SMS key */
    SYS_PHONEBOOK_KEY,      /* Phonebook key */
    SYS_MENU_KEY,           /* Menu key  */
    SYS_RCL_KEY ,           /* Rcl key */
    SYS_DND_MUTE_KEY,       /* DND/MUTE key */
    SYS_SPK_KEY,            /* SPK key */

    SYS_RECORD_KEY,         /* Record key */

    SYS_MP3_PLAY_KEY,       /* MP3 play key */
    SYS_MP3_BACKWARD_KEY,   /* MP3 Backward key */
    SYS_MP3_FORWARD_KEY,    /* MP3 Forward key */



    SYS_RESERVED_1_KEY,     /* resever 1 key */
    SYS_RESERVED_2_KEY,     /* resever 2 key */
    SYS_RESERVED_3_KEY,     /* resever 3 key */
    SYS_RESERVED_4_KEY,     /* resever 4 key */
    SYS_RESERVED_5_KEY,     /* resever 4 key */

    SYS_SHIFT_SPACE_KEY,  /*  shift + space  key*/

    /********************************************************************
    * do not change the above key defintions                            *
    * if you have new hardware key, please define them following        *
    ********************************************************************/

    /********************************************************************
    * SPECIAL KEY START in single key mode
    * FLIP KEY - NO HOLD, if it is press, then other key can be press
    * HOOK KEY - NO HOLD, if it is press, then other key can be press
    *********************************************************************/
    //zhangfanghui modify it from 0x70 to 0x80
    SYS_SPECIAL_KEY_ID_START     = 0x80,            /*do not modify this value*/
    SYS_FLIP_KEY    = SYS_SPECIAL_KEY_ID_START,     /* flip key, do not modify it */
    SYS_HOOK_KEY    = SYS_SPECIAL_KEY_ID_START + 1, /* Hook key */

    /************************************************************************
    * do not change the above special key defintions                        *
    * if you have new special hardware key, please define them following    *
    *************************************************************************/

    SYS_KEY_ID_END  = 0x80                          /*do not modify this value*/
} SysKeyIdT;


#endif
