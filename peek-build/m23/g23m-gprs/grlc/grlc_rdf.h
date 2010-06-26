/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for service RD.
+----------------------------------------------------------------------------- 
*/ 

#ifndef GRLC_RDF_H
#define GRLC_RDF_H


EXTERN void   rd_init ( void );


EXTERN void   rd_tbf_init ( void );


EXTERN BOOL   rd_read_li_m_of_block (  UBYTE * ptr_data_i, UBYTE e_bit_i  );


EXTERN void   rd_out_grlc_data_ind( void );


EXTERN void   rd_out_grlc_data_ind_test( void );


EXTERN void   rd_send_grlc_data_ind ( UBYTE bsn_i );


EXTERN BOOL   rd_check_window_size ( UBYTE bsn_i );


EXTERN void   rd_save_block ( UBYTE bsn_i, UBYTE * ptr_data_block_i, UBYTE fbi_i );


EXTERN void   rd_comp_rec_par ( UBYTE bsn_i );


EXTERN UBYTE  rd_check_fbi ( UBYTE fbi_i, UBYTE sp, ULONG fn, UBYTE rrbp );


EXTERN UBYTE*  rd_set_acknack ( void );


EXTERN UBYTE  rd_calc_rlc_data_len ( USHORT  block_status_i );


EXTERN void   rd_fill_blocks ( UBYTE bsn_i );


EXTERN ULONG  rd_calc_delta_fn ( ULONG fn_i );


EXTERN void rd_free_desc_list_partions ( void );


EXTERN void rd_free_database_partions ( void );


EXTERN void rd_cgrlc_st_time_ind  ( void );

#endif /* !GRLC_RDF_H */

