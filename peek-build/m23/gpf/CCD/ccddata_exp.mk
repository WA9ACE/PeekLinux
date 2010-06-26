#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccddata_exp.mk
#----------------------------------------------------------------------------- 
#  Copyright 2002 Texas Instruments Berlin, AG 
#                 All rights reserved. 
# 
#                 This file is confidential and a trade secret of Texas 
#                 Instruments Berlin, AG 
#                 The receipt of or possession of this file does not convey 
#                 any rights to reproduce or disclose its contents or to 
#                 manufacture, use, or sell anything it may describe, in 
#                 whole, or in part, without the specific written consent of 
#                 Texas Instruments Berlin, AG. 
#----------------------------------------------------------------------------- 
#  Purpose : Define exports of ccddata_dll.dll and ccddata_load.dll.
#----------------------------------------------------------------------------- 
LINK_EXPORTS=
# ccddata_pdi.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_pdi_dinfo
# ccddata_mconst.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_num_of_entities
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_message_id
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_bitstream_len
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_mstruct_len
   LINK_EXPORTS+=/EXPORT:ccddata_mccd_symbols
# ccddata_pconst.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_sap_num
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_primitive_id
   LINK_EXPORTS+=/EXPORT:ccddata_get_max_pstruct_len
# ccddata_ccdmtab.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_mvar
   LINK_EXPORTS+=/EXPORT:ccddata_get_spare
   LINK_EXPORTS+=/EXPORT:ccddata_get_calc
   LINK_EXPORTS+=/EXPORT:ccddata_get_mcomp
   LINK_EXPORTS+=/EXPORT:ccddata_get_melem
   LINK_EXPORTS+=/EXPORT:ccddata_get_mmtx
   LINK_EXPORTS+=/EXPORT:ccddata_get_calcidx
# ccddata_ccdptab.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_pvar
   LINK_EXPORTS+=/EXPORT:ccddata_get_pcomp
   LINK_EXPORTS+=/EXPORT:ccddata_get_pelem
   LINK_EXPORTS+=/EXPORT:ccddata_get_pmtx
# ccddata_cdemval.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_mval
# ccddata_cdemstr.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_mstr
# ccddata_cdepval.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_pval
   LINK_EXPORTS+=/EXPORT:ccddata_get_pstr
# ccd_config.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_mi_length
   LINK_EXPORTS+=/EXPORT:ccddata_get_decmsgbuffer
# ccddata_tap_priv.c
   LINK_EXPORTS+=/EXPORT:ccddata_tap_check_pd
   LINK_EXPORTS+=/EXPORT:ccddata_tap_get_pd
# ccddata_ccdent.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_ccdent
   LINK_EXPORTS+=/EXPORT:ccddata_get_entname
# ccddata_alias.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_alias
# ccddata_version.c
   LINK_EXPORTS+=/EXPORT:ccddata_get_version
   LINK_EXPORTS+=/EXPORT:ccddata_get_table_version
# ccddata_eg.c
   LINK_EXPORTS+=/EXPORT:ccddata_eg_nodes
   LINK_EXPORTS+=/EXPORT:ccddata_eg_nodenames
   LINK_EXPORTS+=/EXPORT:ccddata_eg_adjacent
   LINK_EXPORTS+=/EXPORT:ccddata_eg_saps
   LINK_EXPORTS+=/EXPORT:ccddata_eg_sapnames
   LINK_EXPORTS+=/EXPORT:ccddata_eg_comendpoints
# ccdedit.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_init
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_comp
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_prim_first
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_prim_next
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_msg_first
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_msg_next
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_comp_first
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_comp_next
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_symval
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_read_elem
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_write_prepare
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_write_elem
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_type
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_primcode
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_msgcode
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_is_downlink
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_comp_index
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_element_name
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)cde_get_array_kind

   OFT_EXPORTS:=$(LINK_EXPORTS)
# pdi.obj
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_createDefContext
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_createContext
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_destroyContext
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_startPrim
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_getDecodeInfo
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_getEntityByPD
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pdi_pd2name

# ccd.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_set_patch_infos
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_init
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_begin
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_end
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_codeMsg
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_codeMsgPtr
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_decodeMsg
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_decodeMsgPtr
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_exit
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_init_ccddata
# cdc_std.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_decodeByte
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_codeByte
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_codeLong
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_decodeLong
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_bitcopy
# ccd_err.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_getFirstError
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_getNextError
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_getFirstFault
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_getNextFault
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_free_faultlist
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_get_numFaults
# ccd_elem.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_decodeElem
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)ccd_encodeElem
# pcon.c
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_init_prim_coding
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_decodePrim
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_codePrim
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_init_ccddata
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_make_filter
   LINK_EXPORTS+=/EXPORT:$(DLL_PREFIX)pcon_filter_decode_prim
