#ifndef _KEYMAP_H_
#define _KEYMAP_H_
#if 0
#include "syskeydef.h"

typedef enum
{
  KP_NO_KEY           = 0x0,                    /*!< No Key                                  */

  KP_SHIFT            = SYS_SHIFT,       //shift
  //KP_SHIFT    = SYS_SHIFT,      // shift key as left soft key,for test
                                        //
  KP_QUOTE_KEY        = SYS_QUOTE ,      //' key
  //KP_RIGHT_SOFT_KEY   = SYS_QUOTE,     //' key as right soft key,for test

  KP_DOT_KEY           = SYS_DOT,       //. key
  //KP_CENTER_SOFT_KEY   = SYS_DOT,         // . key as center soft key,for test

  KP_VOLUME_UP_KEY    = 0x80,      //not need.
  KP_VOLUME_DOWN_KEY  = 0x81,    //not need

  KP_LOCK_KEY             = SYS_LOCK,             //lock key
  //KP_UP_KEY           = SYS_LOCK,             // lock key as up key for test.

  KP_SPACE_KEY         = SYS_SPACE,          //space key
  //KP_DOWN_KEY         = SYS_SPACE,           //space key as down key for test.

  KP_REDUCE_KEY         = SYS_REDUCE ,          //-
  //KP_LEFT_KEY         = SYS_REDUCE,           //- key as left key,for test

  KP_COMMA_KEY        = SYS_COMMA,          //, KEY
  //KP_RIGHT_KEY        = SYS_COMMA,          //, key as right key for test

  KP_0_KEY            = SYS_0_KEY,              /*!< '0' key, ASCII '0'                      */
  KP_1_KEY            = SYS_1_KEY,              /*!< '1' key, ASCII '1'                      */
  KP_2_KEY            = SYS_2_KEY,              /*!< '2' key, ASCII '2'                      */
  KP_3_KEY            = SYS_3_KEY,              /*!< '3' key, ASCII '3'                      */
  KP_4_KEY            = SYS_4_KEY,              /*!< '4' key, ASCII '4'                      */
  KP_5_KEY            = SYS_5_KEY,              /*!< '5' key, ASCII '5'                      */
  KP_6_KEY            = SYS_6_KEY,              /*!< '6' key, ASCII '6'                      */
  KP_7_KEY            = SYS_7_KEY,              /*!< '7' key, ASCII '7'                      */
  KP_8_KEY            = SYS_8_KEY,              /*!< '8' key, ASCII '8'                      */
  KP_9_KEY            = SYS_9_KEY,              /*!< '9' key, ASCII '9'                      */
 // KP_POUND_KEY        = 0x82,          //not need
 // KP_STAR_KEY         = 0x83,           //not need

  KP_LEFT_SOFT_KEY    = 0x82,           //not need
  KP_RIGHT_SOFT_KEY   = 0x83,           //not need
  KP_SEND_KEY         = 0x84,           //not need
  KP_END_KEY          = 0x85,            //not need  */


  KP_CLR_KEY          = 0x87,            //not need
  KP_SELECT_KEY       = SYS_ENTER,                   //not need

  KP_MENU_KEY         = 0x89,                   //not need
  KP_SMS_KEY          = 0x90,                   //not need
  KP_PHONEBOOK_KEY    = 0x91,                   //not need
  KP_RCL_KEY          = 0x92,                   //not need
  KP_DND_MUTE_KEY     = 0x93,                   //not need
  KP_SPK_KEY          = 0x94,                   //not need
  KP_HOOK_DOWN_KEY    = 0x95,                   //not need
  KP_HOOK_UP_KEY      = 0x96,                   //not need
  KP_HOOK_HOLD_KEY    = 0x97,                   //not need
  KP_FUNCTION_KEY     = 0x98,                   //not need
  KP_RECORD_KEY       = 0x99,                   //not need

  KP_FLIP_KEY         = SYS_FLIP_KEY,           /*!< Flip detect key                         */

  KP_HEADSET_KEY      = 0xa0,                   //not need
  KP_CAMERA_KEY       = 0xa1,                   //not need

  KP_MP3_PLAY_KEY     = 0xa2,                   //not need
  KP_MP3_FORWARD_KEY  = 0xa3,                   //not need
  KP_MP3_BACKWARD_KEY = 0xa4,                   //not need
  KP_CENTER_SOFT_KEY  = 0xa5,//not need
  KP_UP_KEY           = 0xa6,//not need
  KP_DOWN_KEY         = 0xa7,//not need
  KP_LEFT_KEY         = 0xa8,//not need
  KP_RIGHT_KEY        = 0xa9,//not need


    //zhangfanghui add new keypad interface.
  KP_a_KEY            = SYS_a_KEY,                   //a
  KP_b_KEY            = SYS_b_KEY,                   //b
  KP_c_KEY            = SYS_c_KEY,                   //c
  KP_d_KEY            = SYS_d_KEY,                   //d
  KP_e_KEY            = SYS_e_KEY,
  KP_f_KEY            = SYS_f_KEY,
  KP_g_KEY            = SYS_g_KEY,
  KP_h_KEY            = SYS_h_KEY,
  KP_i_KEY            = SYS_i_KEY,
  KP_j_KEY            = SYS_j_KEY,
  KP_k_KEY            = SYS_k_KEY,
  KP_l_KEY            = SYS_l_KEY,
  KP_m_KEY            = SYS_m_KEY,
  KP_n_KEY            = SYS_n_KEY,
  KP_o_KEY            = SYS_o_KEY,
  KP_p_KEY            = SYS_p_KEY,
  KP_q_KEY            = SYS_q_KEY,
  KP_r_KEY            = SYS_r_KEY,
  KP_s_KEY            = SYS_s_KEY,
  KP_t_KEY            = SYS_t_KEY,
  KP_u_KEY            = SYS_u_KEY,
  KP_v_KEY            = SYS_v_KEY,
  KP_w_KEY            = SYS_w_KEY,
  KP_x_KEY            = SYS_x_KEY,
  KP_y_KEY            = SYS_y_KEY,
  KP_z_KEY            = SYS_z_KEY,                //z


  KP_A_KEY            = SYS_A_KEY,                   //A
  KP_B_KEY            = SYS_B_KEY,                   //B
  KP_C_KEY            = SYS_C_KEY,                   //C
  KP_D_KEY            = SYS_D_KEY,                   //D
  KP_E_KEY            = SYS_E_KEY,
  KP_F_KEY            = SYS_F_KEY,
  KP_G_KEY            = SYS_G_KEY,
  KP_H_KEY            = SYS_H_KEY,
  KP_I_KEY            = SYS_I_KEY,
  KP_J_KEY            = SYS_J_KEY,
  KP_K_KEY            = SYS_K_KEY,
  KP_L_KEY            = SYS_L_KEY,
  KP_M_KEY            = SYS_M_KEY,
  KP_N_KEY            = SYS_N_KEY,
  KP_O_KEY            = SYS_O_KEY,
  KP_P_KEY            = SYS_P_KEY,
  KP_Q_KEY            = SYS_Q_KEY,
  KP_R_KEY            = SYS_R_KEY,
  KP_S_KEY            = SYS_S_KEY,
  KP_T_KEY            = SYS_T_KEY,
  KP_U_KEY            = SYS_U_KEY,
  KP_V_KEY            = SYS_V_KEY,
  KP_W_KEY            = SYS_W_KEY,
  KP_X_KEY            = SYS_X_KEY,
  KP_Y_KEY            = SYS_Y_KEY,
  KP_Z_KEY            = SYS_Z_KEY,                //Z

  KP_ENTER_KEY        = 0x88,                //enter key.
  KP_AT_KEY           = SYS_AT,                   //@
  KP_BACKSPACE_KEY    = SYS_BACKSPACE,             //backspace
  KP_WHEEL_UP         = SYS_WHEEL_BACK,
  KP_WHEEL_DOWN       = SYS_WHEEL_FORWARD,
  KP_CANCEL_KEY       = SYS_CANCEL_KEY,
  KP_PWR_KEY          = SYS_PWR_KEY,

  KP_EXCAL_KEY        =  SYS_EXCAL_KEY,            //excalmatory point,!
  KP_QUESTION_KEY     =  SYS_QUESTION_KEY,         //question mark,?
  KP_WHEEL            = SYS_WHEEL,
  KP_POUND_KEY        =  SYS_POUND_KEY,          // '#' key
  KP_DOLLOR_KEY       =  SYS_DOLLAR_KEY,         //$ key
  KP_PERCENT_KEY      =  SYS_PERCENT_KEY,            //% key
  KP_SLASH_KEY        =  SYS_SLASH_KEY,          // '/' key
  KP_AND_KEY          =  SYS_AND_KEY,            // & key
  KP_STAR_KEY         =  SYS_STAR_KEY,           // '*' key
  KP_LEFT_BRACKET_KEY =  SYS_LEFT_BRACKET_KEY,   //( key
  KP_RIGHT_BRACKET_KEY=  SYS_RIGHT_BRACKET_KEY,  //) key

  KP_COLON_KEY        =  SYS_COLON_KEY,          // : key
  KP_SEMICOLON_KEY    =  SYS_SEMICOLON_KEY,      //; key
  KP_DOUBLE_QUESTIONMASKS=  SYS_DOUBLE_QUESTIONMASKS_KEY, // " key
  KP_UNDERLINE_KEY    =  SYS_UNDERLINE_KEY,          // _ key
  KP_EQUAL_KEY        =  SYS_EQUAL_KEY,              //= key
  KP_WHEEL_SHIFT_KEY  =  SYS_WHEEL_SHIFT,
  KP_WHEEL_UP_SHIFT   =  SYS_WHEEL_BACK_SHIFT,
  KP_WHEEL_DOWN_SHIFT =  SYS_WHEEL_FORWARD_SHIFT,
  KP_SHIFT_SPACE_KEY =  SYS_SHIFT_SPACE_KEY
} UiKeypadKeyIdT;

#endif
#endif

