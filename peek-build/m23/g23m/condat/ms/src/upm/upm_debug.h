/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    Debug definitions for the User Plane Management (UPM) Entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef UPM_DEBUG_H
#define UPM_DEBUG_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

#ifdef S_SPLINT_S  /* LINT code checker defines */
#undef TRUE
#undef FALSE
typedef /*@only@*/ T_VOID_STRUCT *T_VOID_STRUCT_ONLY;
#ifdef TI_UMTS
typedef /*@null@*/ /*@only@*/
T_MEM_uplink_buffer_users_info *lint_mem_user_info;
#endif /* TI_UMTS */
#ifndef UPM_WITHOUT_USER_PLANE
typedef /*@null@*/ /*@only@*/ T_desc2 *lint_desc_ptr;
/*@-redef@*/ /*@-matchfields@*/
typedef struct
{
  /*@null@*/ U32 /*@alt lint_desc_ptr@*/ next;
  U16                         offset;
  U16                         len;
  U16                         size;
  /*@reldef@*/ U8             buffer[1];
  /*@reldef@*//*@unused@*/ U8 zzz_align0;
} T_desc2;
typedef struct
{
  U16                         list_len;
  /*@reldef@*//*@unused@*/ U8 zzz_align0;
  /*@reldef@*//*@unused@*/ U8 zzz_align1;
  U32 /*@alt lint_desc_ptr@*/ first;
} T_desc_list2;
/*@=redef@*/ /*@=matchfields@*/
#endif /* UPM_WITHOUT_USER_PLANE */
#endif /* S_SPLINT_S */

/*==== EXPORTS ==============================================================*/

#ifdef S_SPLINT_S  /* LINT code checker defines */
/*@-incondefs@*/ /*@-redecl@*/
T_HANDLE           vsi_c_open   (T_HANDLE Caller, /*@observer@*/ char *Name)
     /*@globals internalState@*/ /*@modifies internalState@*/;
int /*@alt void@*/ vsi_c_close  (T_HANDLE Caller, T_HANDLE ComHandle);

int /*@alt void@*/ vsi_c_psend  (T_HANDLE ComHandle,
                                 /*@only@*/ /*@out@*/T_VOID_STRUCT *ptr FILE_LINE_TYPE);

/*@null@*/ /*@only@*/ /*@out@*/ T_VOID_STRUCT *
                   vsi_c_pnew   (U32 /*@alt int@*/        Size,
                                 U32 /*@alt int@*/        opc FILE_LINE_TYPE);
/*@null@*/ /*@only@*/ /*@out@*/ T_VOID_STRUCT *
                   vsi_c_new_sdu(U32 /*@alt int@*/        Size,
                                 U32 /*@alt int@*/        opc,
                                 U16 /*@alt int,size_t@*/ sdu_len, 
                                 U16 /*@alt int,size_t@*/ sdu_offset,
                                 U16 /*@alt int@*/        encode_offset
                                 FILE_LINE_TYPE);
int /*@alt void@*/ vsi_c_pfree   (/*@out@*/ /*@null@*/
                                  T_VOID_STRUCT_ONLY     *Msg  FILE_LINE_TYPE);
/*@null@*/ /*@only@*/ /*@out@*/ T_VOID_STRUCT *
                   vsi_m_cnew   (U32 /*@alt int@*/        size,
                                 U32 /*@alt int@*/        type FILE_LINE_TYPE)
     /*@globals internalState@*/ /*@modifies internalState@*/;
int /*@alt void@*/ vsi_m_cfree   (/*@out@*/ /*@null@*/
                                  T_VOID_STRUCT_ONLY     *ptr  FILE_LINE_TYPE)
     /*@globals internalState@*/ /*@modifies internalState@*/;

#ifdef TI_UMTS
void               mem_adjust_uplink_users_flow_control(/*@in@*/ lint_mem_user_info users_info[MEM_MAX_RAB_ID]);

/*@owned@*/U8 *    mem_alloc    (U8 user_handle, U16 length,
                                 /*@out@*/ U32           *ptr_handle);
/*@owned@*/U8 *    mem_get_user_data(T_MEM_HANDLE         memory_handle,
                                 /*@out@*/ U16           *ptr_length);
#endif /* TI_UMTS */

#ifndef UPM_WITHOUT_USER_PLANE
DTI_HANDLE         dti_init     (U8  maximum_links, T_HANDLE handle,
                                 U32 entity_options,
/*@-type@*/                      void (*sig_callback)
/*@=type@*/                      (U8 instance, U8 interfac, U8 channel, U8 reason,
                                  /*@only@*/ /*@null@*/
                                  T_DTI2_DATA_IND         *dti_data_ind))
     /*@globals internalState@*/ /*@modifies internalState@*/;

BOOL               dti_resolve_link_id(/*@observer@*/DTI_HANDLE hDTI,
                                 U8 instance, U8 interfac, U8 channel,
                                 /*@out@*/ U32 *link_id);

void               mfree_desc   (DTI_HANDLE hDTI,
                                 /*@null@*/ /*@out@*/
                                 T_desc_list2 * desc_list2)
     /*@ensures isnull desc_list2->first@*/
     /*@globals internalState@*/ /*@modifies internalState@*/;

void               dti_send_data(/*@observer@*/DTI_HANDLE hDTI,
                                 U8 instance, U8 interfac, U8 channel,
                                 /*@only@*/ /*@null@*/ /*@out@*/
                                 T_DTI2_DATA_IND *dti_data_ind);
#endif /* UPM_WITHOUT_USER_PLANE */

T_CL_result /*@alt void@*/
cl_qos_convert_r97_to_r99(/*@partial@*//*@in@*/T_PS_qos_r97 *src_qos_r97,
                          /*@out@*/            T_PS_qos_r99 *dst_qos_r99);
T_CL_result /*@alt void@*/
cl_qos_convert_r99_to_r97(/*@partial@*//*@in@*/T_PS_qos_r99 *src_qos_r99,
                          /*@out@*/            T_PS_qos_r97 *dst_qos_r97);
/*@=incondefs@*/ /*@=redecl@*/
#endif /* S_SPLINT_S */

#endif /* UPM_DEBUG_H */
/*==== END OF FILE ==========================================================*/
