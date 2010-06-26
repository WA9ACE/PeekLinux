// THIS FILE IS ONLY VALID FOR LOCOSTO PLUS BOARDS
// (BOARD==71)
// (CHIPSET==15)
// (LOCOSTO_LITE==0)
// (LONG_JUMP==3)
// (GSM_IDLE_RAM==0)
// (GSMLITE==0)
// (REMU==1)


// l1_intram.o
(BSS_LIBS (.bss, .l1s_global) CONST_LIBS (.const, .text))

// drivers_int_ram.a
(CONST_LIBS (.loop) LOOP_LIB (.loop))

// tpudrv.a
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))

// l1_int.a
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))

// l1_custom_int.a
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))



// l1_ext.a
(BSS_LIBS (.l1s_global, API_HISR_global))


   // init
(BSS_LIBS (API_HISR_stack))

// IB : is it useful  : used at component level ?
#if ((R2D_STATE==1) || (R2D_STATE==2))
// r2d_drv_int_ram
(CONST_LIBS (.text))
   // r2d_drv_int_ram
(CONST_LIBS (.r2dlcd))
#endif

// IB : is it useful  : used at component level ?
#if (CAMD_STATE==2)
   // camd_test
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))
#endif

// IB : is it useful  : used at component level ?
#if ((OP_L1_STANDALONE==0) && (WCP_PROF==1))
// prf
(CONST_LIBS (.text, .const))
   // uart_drv
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
#endif

// dar_gbl_var.a
#if ((DAR_STATE==1))
(BSS_DAR_LIB (.bss))
#endif

// IB : is it useful  : used at component level ?
   // ti_profiler
#if ((TI_PROFILER==1) || (TI_NUC_MONITOR==1))
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
#endif

// audio_int_ram.a
//#if ((L1_EXT_AUDIO_MGT==1) && ((AUDIO_STATE==1) || (AUDIO_STATE==2)))
#if (((AUDIO_STATE==1) || (AUDIO_STATE==2)))
(AUDIO_BSS_LIBS (.bss))
#endif

#if ((BAE_STATE == 1) || ( BAE_STATE == 2))
// bae_sram.a
(CONST_LIBS (.text, .const))
// bae_sram.a
(CONST_LIBS (BAE_ASM_THUMB, BAE_ASM_ARM))
//#endif
#endif

#if (JPEGCODEC_STATE==1)
// jpeg_intram.a
(CONST_LIBS (.text))
#endif

// rts16le_int_ram.a
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))

// nucleus_int_ram.a
(BSS_LIBS (.bss) CONST_LIBS (.const, .text))

// gpf osx
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
// gpf frame
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
// gpf misc
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
// gpf tif
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))


#if ((PMODE==2) && (WAP==0)||(AAA_DAA==1))
// ppp_ir.a
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
#endif
#if (PMODE==2)
   // cci_ir
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
   // grlc_ir
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
//config_gprs_ir
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))
#endif

// remu int lib.a
(BSS_LIBS (.bss) CONST_LIBS (.text, .const))


   // multimedia fix
#if ((BAE_STATE == 1) || (BAE_STATE == 2))

   // bae.a
(BAE_LINK_LIBS (.text))

#endif
   // as.a
(BAE_LINK_LIBS (.text))
   // audio.a
(BAE_LINK_LIBS (.text))
   // rfs.a
(BAE_LINK_LIBS (.text))
   // frame_na7_db_fl_ts3.lib
(BAE_LINK_LIBS (.text))
   // rts16le_int_ram.lib
(BAE_LINK_LIBS (.text))
   // remu_na7_db_ts3.lib
(BAE_LINK_LIBS (.text))


