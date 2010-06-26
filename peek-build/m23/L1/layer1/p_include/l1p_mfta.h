/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_MFTAB.H
 *
 *        Filename l1p_mfta.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#define L1P_MFTAB_H

// Prototypes
void l1s_hopping_algo     (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_single     (UWORD8 param1, UWORD8 param2);
void l1ps_read_single     (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_snb_dl     (UWORD8 param1, UWORD8 param2);
void l1ps_read_nb_dl      (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_pdtch      (UWORD8 param1, UWORD8 param2);
void l1ps_read_pdtch      (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_pbcch      (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_prach      (UWORD8 param1, UWORD8 param2);
void l1ps_read_pra_result (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_poll       (UWORD8 param1, UWORD8 param2);
void l1ps_read_poll_result(UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_ptcch      (UWORD8 param1, UWORD8 param2);
void l1ps_read_ptcch      (UWORD8 param1, UWORD8 param2);

void l1ps_ctrl_itmeas     (UWORD8 param1, UWORD8 param2);
void l1ps_read_itmeas     (UWORD8 param1, UWORD8 param2);

/***********************************************************
 * Content:
 *   This file contains the MultiFrame tables for all L1S
 *   Packet basic tasks.
 ***********************************************************/

/*******************************************************************************************/
/* Multiframe Blocks for Dynamic MFTAB Building purpose.                                   */
/*******************************************************************************************/
// Multiframe table size....
#define BLOC_PDTCH_SIZE         6       // PDTCH.
#define BLOC_PRACH_SIZE         3       // PRACH.
#define BLOC_POLL_SIZE          6       // POLL.
#define BLOC_POLL_NO_HOPP_SIZE  6       // POLL without hopping algo called.
#define BLOC_SINGLE_SIZE        6       // SINGLE.
#define BLOC_PCCCH_SIZE         6       // PNP, PEP and PALLC
#define BLOC_PBCCHS_SIZE        6       // Serving Cell PBCCH
#define BLOC_PTCCH_SIZE         3       // Serving Cell PTCCH
#define BLOC_ITMEAS_SIZE        4       // Interference measurements
#define BLOC_PBCCHN_TRAN_SIZE   6       // Neighbor Cell PBCCH in packet Transfer
#define BLOC_PBCCHN_IDLE_SIZE   8       // Neighbor Cell PBCCH in Idle


#ifdef L1P_ASYN_C
  /*----------------------------------------------------*/
  /* TASK: Packet Normal Paging...                      */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     -> hopping + burst 1         */  
  /*         C W R | |     -> hopping + burst 2         */  
  /*           C W R |     -> hopping + burst 3         */  
  /*             C W R     -> hopping + burst 4         */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_PNP[] =      
  { 
    {l1s_hopping_algo,PNP,NO_PAR},{l1ps_ctrl_snb_dl,PNP,BURST_1},                              {NULL,NO_PAR,NO_PAR},  // frame 1
    {l1s_hopping_algo,PNP,NO_PAR},{l1ps_ctrl_snb_dl,PNP,BURST_2},                              {NULL,NO_PAR,NO_PAR},  // frame 2
    {l1s_hopping_algo,PNP,NO_PAR},{l1ps_read_nb_dl,PNP,BURST_1},{l1ps_ctrl_snb_dl,PNP,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PNP,NO_PAR},{l1ps_read_nb_dl,PNP,BURST_2},{l1ps_ctrl_snb_dl,PNP,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                  {l1ps_read_nb_dl,PNP,BURST_3},{NULL,NO_PAR,NO_PAR},                                // frame 5
                                  {l1ps_read_nb_dl,PNP,BURST_4},{NULL,NO_PAR,NO_PAR}                                 // frame 6
  }; 

  /*----------------------------------------------------*/
  /* TASK: Packet Extended Paging...                    */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     ->hopping + burst 1          */  
  /*         C W R | |     ->hopping + burst 2          */  
  /*           C W R |     ->hopping + burst 3          */  
  /*             C W R     ->hopping + burst 4          */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_PEP[] =      
  { 
    {l1s_hopping_algo,PEP,NO_PAR},{l1ps_ctrl_snb_dl,PEP,BURST_1},                              {NULL,NO_PAR,NO_PAR},  // frame 1
    {l1s_hopping_algo,PEP,NO_PAR},{l1ps_ctrl_snb_dl,PEP,BURST_2},                              {NULL,NO_PAR,NO_PAR},  // frame 2
    {l1s_hopping_algo,PEP,NO_PAR},{l1ps_read_nb_dl,PEP,BURST_1},{l1ps_ctrl_snb_dl,PEP,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PEP,NO_PAR},{l1ps_read_nb_dl,PEP,BURST_2},{l1ps_ctrl_snb_dl,PEP,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                  {l1ps_read_nb_dl,PEP,BURST_3},{NULL,NO_PAR,NO_PAR},                                // frame 5
                                  {l1ps_read_nb_dl,PEP,BURST_4},{NULL,NO_PAR,NO_PAR}                                 // frame 6
  }; 

  /*----------------------------------------------------*/
  /* TASK: All PCCCH Reading ...                        */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     -> hopping + burst 1         */  
  /*         C W R | |     -> hopping + burst 2         */  
  /*           C W R |     -> hopping + burst 3         */  
  /*             C W R     -> hopping + burst 4         */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_PALLC[] =      
  { 
    {l1s_hopping_algo,PALLC,NO_PAR},{l1ps_ctrl_snb_dl,PALLC,BURST_1},                                {NULL,NO_PAR,NO_PAR},    // frame 1
    {l1s_hopping_algo,PALLC,NO_PAR},{l1ps_ctrl_snb_dl,PALLC,BURST_2},                                {NULL,NO_PAR,NO_PAR},    // frame 2
    {l1s_hopping_algo,PALLC,NO_PAR},{l1ps_read_nb_dl,PALLC,BURST_1},{l1ps_ctrl_snb_dl,PALLC,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PALLC,NO_PAR},{l1ps_read_nb_dl,PALLC,BURST_2},{l1ps_ctrl_snb_dl,PALLC,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                    {l1ps_read_nb_dl,PALLC,BURST_3},{NULL,NO_PAR,NO_PAR},                                    // frame 5
                                    {l1ps_read_nb_dl,PALLC,BURST_4},{NULL,NO_PAR,NO_PAR}                                     // frame 6
  };  

  /*--------------------------------------------------------------*/
  /* TASK: Serving Cell PBCCH task...                             */
  /*--------------------------------------------------------------*/
  /* frame   1 2 3 4 5 6                                          */  
  /*         | | | | | |                                          */  
  /*         C W R | | |     -> hopping + Synch + burst 1         */  
  /*           C W R | |     -> hopping + burst 2                 */  
  /*             C W R |     -> hopping + burst 3                 */  
  /*               C W R     -> hopping + burst 4 + Synch back    */  
  /*--------------------------------------------------------------*/
  const T_FCT  BLOC_PBCCHS[] =      
  {
    {l1s_hopping_algo,PBCCHS,NO_PAR},{l1ps_ctrl_pbcch,PBCCHS,BURST_1},                                 {NULL,NO_PAR,NO_PAR}, // frame 1
    {l1s_hopping_algo,PBCCHS,NO_PAR},{l1ps_ctrl_pbcch,PBCCHS,BURST_2},                                 {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1s_hopping_algo,PBCCHS,NO_PAR},{l1ps_read_nb_dl,PBCCHS,BURST_1},{l1ps_ctrl_pbcch,PBCCHS,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PBCCHS,NO_PAR},{l1ps_read_nb_dl,PBCCHS,BURST_2},{l1ps_ctrl_pbcch,PBCCHS,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                                                      {l1ps_read_nb_dl,PBCCHS,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 5
                                                                      {l1ps_read_nb_dl,PBCCHS,BURST_4},{NULL,NO_PAR,NO_PAR}  // frame 6
  }; 


  /*--------------------------------------------------------------*/
  /* TASK: Neighbor Cell PBCCH task in Packet Transfer mode...    */
  /*--------------------------------------------------------------*/
  /* frame   1 2 3 4 5 6                                          */  
  /*         | | | | | |                                          */  
  /*         C W R | | |     -> hopping + Synch + burst 1         */  
  /*           C W R | |     -> hopping + burst 2                 */  
  /*             C W R |     -> hopping + burst 3                 */  
  /*               C W R     -> hopping + burst 4 + Synch back    */  
  /*--------------------------------------------------------------*/
  const T_FCT  BLOC_PBCCHN_TRAN[] =      
  {
    {l1s_hopping_algo,PBCCHN_TRAN,NO_PAR},{l1ps_ctrl_pbcch,PBCCHN_TRAN,BURST_1},                                      {NULL,NO_PAR,NO_PAR}, // frame 1
    {l1s_hopping_algo,PBCCHN_TRAN,NO_PAR},{l1ps_ctrl_pbcch,PBCCHN_TRAN,BURST_2},                                      {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1s_hopping_algo,PBCCHN_TRAN,NO_PAR},{l1ps_read_nb_dl,PBCCHN_TRAN,BURST_1},{l1ps_ctrl_pbcch,PBCCHN_TRAN,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PBCCHN_TRAN,NO_PAR},{l1ps_read_nb_dl,PBCCHN_TRAN,BURST_2},{l1ps_ctrl_pbcch,PBCCHN_TRAN,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                                                                {l1ps_read_nb_dl,PBCCHN_TRAN,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 5
                                                                                {l1ps_read_nb_dl,PBCCHN_TRAN,BURST_4},{NULL,NO_PAR,NO_PAR}  // frame 6
  }; 

  /*--------------------------------------------------------------*/
  /* TASK: Neighbor Cell PBCCH task in Idle mode...               */
  /*--------------------------------------------------------------*/
  /* frame  1 2 3 4 5 6 7 8                                       */  
  /*        | | | | | | | |                                       */  
  /*        C W R | | | | |     -> AGC                            */  
  /*            C W R | | |     -> hopping + Synch + burst 1      */  
  /*              C W R | |     -> hopping + burst 2              */  
  /*                C W R |     -> hopping + burst 3              */  
  /*                  C W R     -> hopping + burst 4 + Synch back */  
  /*--------------------------------------------------------------*/
  const T_FCT  BLOC_PBCCHN_IDLE[] =      
  {
    {l1s_ctrl_msagc,PBCCHN_IDLE,NO_PAR},   
                                                                                                                                                          {NULL,NO_PAR,NO_PAR}, // frame 1
                                                                                                                                                          {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1s_read_msagc,PBCCHN_IDLE,NO_PAR},{l1s_hopping_algo,PBCCHN_IDLE,NO_PAR},{l1ps_ctrl_pbcch,PBCCHN_IDLE,BURST_1},                                      {NULL,NO_PAR,NO_PAR}, // frame 3
                                        {l1s_hopping_algo,PBCCHN_IDLE,NO_PAR},{l1ps_ctrl_pbcch,PBCCHN_IDLE,BURST_2},                                      {NULL,NO_PAR,NO_PAR}, // frame 4
                                        {l1s_hopping_algo,PBCCHN_IDLE,NO_PAR},{l1ps_read_nb_dl,PBCCHN_IDLE,BURST_1},{l1ps_ctrl_pbcch,PBCCHN_IDLE,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 5
                                        {l1s_hopping_algo,PBCCHN_IDLE,NO_PAR},{l1ps_read_nb_dl,PBCCHN_IDLE,BURST_2},{l1ps_ctrl_pbcch,PBCCHN_IDLE,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 6
                                                                                                                    {l1ps_read_nb_dl,PBCCHN_IDLE,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 7
                                                                                                                    {l1ps_read_nb_dl,PBCCHN_IDLE,BURST_4},{NULL,NO_PAR,NO_PAR}  // frame 8
  }; 

  /*----------------------------------------------------*/
  /* TASK: PDTCH                                        */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     -> hopping + burst 1         */  
  /*         C W R | |     -> hopping + burst 2         */  
  /*           C W R |     -> hopping + burst 3         */  
  /*             C W R     -> hopping + burst 4         */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_PDTCH[] =      
  {
    {l1s_hopping_algo,PDTCH,NO_PAR},{l1ps_ctrl_pdtch,PDTCH,BURST_1},                                {NULL,NO_PAR,NO_PAR}, // frame 1
    {l1s_hopping_algo,PDTCH,NO_PAR},{l1ps_ctrl_pdtch,PDTCH,BURST_2},                                {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1s_hopping_algo,PDTCH,NO_PAR},{l1ps_read_pdtch,PDTCH,BURST_1},{l1ps_ctrl_pdtch,PDTCH,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3
    {l1s_hopping_algo,PDTCH,NO_PAR},{l1ps_read_pdtch,PDTCH,BURST_2},{l1ps_ctrl_pdtch,PDTCH,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                                                    {l1ps_read_pdtch,PDTCH,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 5
                                                                    {l1ps_read_pdtch,PDTCH,BURST_4},{NULL,NO_PAR,NO_PAR}  // frame 6
  };

  /*--------------------------------------------------------*/
  /* TASK: SINGLE                                           */
  /*--------------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                      */  
  /*       | | | | | |                                      */  
  /*       C W R | | |     -> hopping + burst 1 + Sync      */  
  /*         C W R | |     -> hopping + burst 2             */  
  /*           C W R |     -> hopping + burst 3             */  
  /*             C W R     -> hopping + burst 4 + Sync back */  
  /*--------------------------------------------------------*/
  const T_FCT  BLOC_SINGLE[] = 
  { 
    {l1s_hopping_algo,SINGLE,NO_PAR},{l1ps_ctrl_single,SINGLE,BURST_1},                                  {NULL,NO_PAR,NO_PAR}, // frame 1
    {l1s_hopping_algo,SINGLE,NO_PAR},{l1ps_ctrl_single,SINGLE,BURST_2},                                  {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1s_hopping_algo,SINGLE,NO_PAR},{l1ps_read_single,SINGLE,BURST_1},{l1ps_ctrl_single,SINGLE,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 3 
    {l1s_hopping_algo,SINGLE,NO_PAR},{l1ps_read_single,SINGLE,BURST_2},{l1ps_ctrl_single,SINGLE,BURST_4},{NULL,NO_PAR,NO_PAR}, // frame 4
                                                                       {l1ps_read_single,SINGLE,BURST_3},{NULL,NO_PAR,NO_PAR}, // frame 5
                                                                       {l1ps_read_single,SINGLE,BURST_4},{NULL,NO_PAR,NO_PAR}  // frame 6
  }; 
  /*----------------------------------------------------*/
  /* TASK: PRACH task...                                */
  /*----------------------------------------------------*/
  const T_FCT  BLOC_PRACH[] =      
  { 
    {l1ps_ctrl_prach,PRACH,NO_PAR},      {NULL,NO_PAR,NO_PAR}, // frame 1
                                         {NULL,NO_PAR,NO_PAR}, // frame 2
    {l1ps_read_pra_result,PRACH,NO_PAR}, {NULL,NO_PAR,NO_PAR}  // frame 3
  }; 

  /*----------------------------------------------------*/
  /* TASK: POLL task...                                */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     -> burst 1                   */  
  /*         C W R | |     -> burst 2                   */  
  /*           C W R |     -> burst 3                   */  
  /*             C W R     -> burst 4                   */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_POLL[] =      
  { 
    {l1s_hopping_algo,POLL,NO_PAR},{l1ps_ctrl_poll,POLL,BURST_1},                                     {NULL,NO_PAR,NO_PAR},  // frame 1
    {l1s_hopping_algo,POLL,NO_PAR},{l1ps_ctrl_poll,POLL,BURST_2},                                     {NULL,NO_PAR,NO_PAR},  // frame 2
    {l1s_hopping_algo,POLL,NO_PAR},{l1ps_read_poll_result,POLL,BURST_1},{l1ps_ctrl_poll,POLL,BURST_3},{NULL,NO_PAR,NO_PAR},  // frame 3
    {l1s_hopping_algo,POLL,NO_PAR},{l1ps_read_poll_result,POLL,BURST_2},{l1ps_ctrl_poll,POLL,BURST_4},{NULL,NO_PAR,NO_PAR},  // frame 4
                                   {l1ps_read_poll_result,POLL,BURST_3},{NULL,NO_PAR,NO_PAR},                                // frame 5
                                   {l1ps_read_poll_result,POLL,BURST_4},{NULL,NO_PAR,NO_PAR}                                                                // frame 6
  }; 

  /*----------------------------------------------------*/
  /* TASK: POLL_NO_HOPP task...                         */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4 5 6                                  */  
  /*       | | | | | |                                  */  
  /*       C W R | | |     -> burst 1                   */  
  /*         C W R | |     -> burst 2                   */  
  /*           C W R |     -> burst 3                   */  
  /*             C W R     -> burst 4                   */  
  /*----------------------------------------------------*/
  const T_FCT  BLOC_POLL_NO_HOPP[] =      
  { 
    {l1ps_ctrl_poll,POLL,BURST_1},                                     {NULL,NO_PAR,NO_PAR},  // frame 1
    {l1ps_ctrl_poll,POLL,BURST_2},                                     {NULL,NO_PAR,NO_PAR},  // frame 2
    {l1ps_read_poll_result,POLL,BURST_1},{l1ps_ctrl_poll,POLL,BURST_3},{NULL,NO_PAR,NO_PAR},  // frame 3
    {l1ps_read_poll_result,POLL,BURST_2},{l1ps_ctrl_poll,POLL,BURST_4},{NULL,NO_PAR,NO_PAR},  // frame 4
                                   {l1ps_read_poll_result,POLL,BURST_3},{NULL,NO_PAR,NO_PAR},                                // frame 5
                                   {l1ps_read_poll_result,POLL,BURST_4},{NULL,NO_PAR,NO_PAR}                                                                // frame 6
  }; 


  /*--------------------------------------------------------*/
  /* TASK: PTCCH                                            */
  /*--------------------------------------------------------*/
  /* frame 1 2 3 4                                          */  
  /*       | | | |                                          */  
  /*       C W W R                                          */  
  /*--------------------------------------------------------*/
  const T_FCT  BLOC_PTCCH[] = 
  { 
    {l1s_hopping_algo,PTCCH,NO_PAR},{l1ps_ctrl_ptcch,PTCCH,NO_PAR},{NULL,NO_PAR,NO_PAR}, // frame 1
                                                                   {NULL,NO_PAR,NO_PAR}, // frame 2
                                    {l1ps_read_ptcch,PTCCH,NO_PAR},{NULL,NO_PAR,NO_PAR}  // frame 3
  }; 

  /*----------------------------------------------------*/
  /* TASK: Interference measurements...                 */
  /*----------------------------------------------------*/
  /* frame 1 2 3 4                                      */  
  /*       | | | |                                      */  
  /*       C W W R         -> hopping + measurements    */   
  /*----------------------------------------------------*/
  const T_FCT  BLOC_ITMEAS[] =      
  { 
    {l1s_hopping_algo,ITMEAS,NO_PAR},{l1ps_ctrl_itmeas,NO_PAR,NO_PAR},{NULL,NO_PAR,NO_PAR}, // frame 1
                                                                      {NULL,NO_PAR,NO_PAR}, // frame 2
                                                                      {NULL,NO_PAR,NO_PAR}, // frame 2
                                     {l1ps_read_itmeas,NO_PAR,NO_PAR},{NULL,NO_PAR,NO_PAR}  // frame 4
  }; 


#else                                                                         
  extern T_FCT  BLOC_PNP[]; 
  extern T_FCT  BLOC_PEP[];
  extern T_FCT  BLOC_PALLC[];
  extern T_FCT  BLOC_PBCCHS[]; 
  extern T_FCT  BLOC_PBCCHN_TRAN[]; 
  extern T_FCT  BLOC_PBCCHN_IDLE[];                                                                                                                                             
  extern T_FCT  BLOC_PDTCH[];      
  extern T_FCT  BLOC_SINGLE[];      
  extern T_FCT  BLOC_PRACH[];      
  extern T_FCT  BLOC_POLL[]; 
  extern T_FCT  BLOC_POLL_NO_HOPP[];  
  extern T_FCT  BLOC_PTCCH[];
  extern T_FCT  BLOC_ITMEAS[];
#endif
