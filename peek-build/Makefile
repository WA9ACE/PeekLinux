include Makefile.globals

TARGET    = $(BUILDDIR)/peek.bin

MOTOROLA  = $(BUILDDIR)/rel_n5_camera_micron_vin_merge.m0
TARGETOUT = $(BUILDDIR)/rel_n5_camera_micron_vin_merge.out

LINKOUT  = $(BUILDDIR)/rel_n5_camera_micron_vin.out
LINKMAP  = $(BUILDDIR)/rel_n5_camera_micron_vin.map
LINKCMD  = $(BUILDDIR)/rel_n5_camera_micron_vin.cmd

BASEBAND = $(BASEDIR)/cert/drp_fw.out 

CMD_TOOL = $(BASEDIR)/tools/make_cmd.pl
EXTRACT  = $(BASEDIR)/tools/extractBin.sh

LCFSRC     = $(BASEDIR)/tools/lcfgen.c
CMD_OPT_PP = $(BUILDDIR)/lcfgen.pp

UNDEFINE += OP_L1_STANDALONE
UNDEFINE += WCP_PROF

CMD_MAP  = $(LIBSDIR)/chipsetsw/system/gsm_is_lj3.template 
CMD_MAP += $(LIBSDIR)/layer1/l1_intram.o 
CMD_MAP += $(LIBSDIR)/csw-system/drv_core/clkm/drivers_int_ram.a 
CMD_MAP += $(LIBSDIR)/layer1/tpudrv.a 
CMD_MAP += $(LIBSDIR)/layer1/l1_int.a 
CMD_MAP += $(LIBSDIR)/layer1/l1_custom_int.a 
CMD_MAP += $(LIBSDIR)/layer1/l1_ext.a 
CMD_MAP += $(LIBSDIR)/csw-system/init.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/dar_gbl_var.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/audio_int_ram.a 
CMD_MAP += $(LIBSDIR)/thirdparty/bae_sram.a 
CMD_MAP += $(LIBSDIR)/thirdparty/bae_sram.a 
CMD_MAP += $(LIBSDIR)/thirdparty/jpeg_intram.a 
CMD_MAP += $(LIBSDIR)/csw-system/rtslib/rts16le_int_ram.a 
CMD_MAP += $(LIBSDIR)/csw-system/nucleus/nucleus_int_ram.a 
CMD_MAP += $(LIBSDIR)/gpf/osx_na7_db.lib 
CMD_MAP += $(LIBSDIR)/gpf/frame_na7_db_ir.lib 
CMD_MAP += $(LIBSDIR)/gpf/misc_na7_db_ir.lib 
CMD_MAP += $(LIBSDIR)/gpf/tif_na7_db_ir.lib 
CMD_MAP += $(LIBSDIR)/g23m-fad/ppp_ir.a 
CMD_MAP += $(LIBSDIR)/g23m-gprs/cci_ir.a 
CMD_MAP += $(LIBSDIR)/g23m-gprs/grlc_ir.a 
CMD_MAP += $(LIBSDIR)/g23m/config_gprs_ir.a 
CMD_MAP += $(LIBSDIR)/g23m/remu_int_ram.a 
CMD_MAP += $(LIBSDIR)/thirdparty/bae.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/as.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/audio.a 
CMD_MAP += $(LIBSDIR)/csw-system/drv_app/rfs.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/system/rts16le_int_ram.lib 
CMD_MAP += $(LIBSDIR)/gpf/remu_na7_db_ts3.lib 
CMD_MAP += $(LIBSDIR)/gpf/frame_na7_db_fl_ts3.lib

LINK_OBJ  = $(BUILDDIR)/str2ind.o 
LINK_OBJ += $(LIBSDIR)/thirdparty/jpeg_codec.a 
LINK_OBJ += $(LIBSDIR)/thirdparty/jpeg_intram.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_ext.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_int.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_intram.o
LINK_OBJ += $(LIBSDIR)/layer1/l1_custom_int.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_custom_ext.a
LINK_OBJ += $(LIBSDIR)/layer1/tpudrv.a
LINK_OBJ += $(LIBSDIR)/bmi/bmi.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/audio.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/audio_bgd.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/audio_int_ram.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/as.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/cst.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/dar.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/dar_gbl_var.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/etm.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/img.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/lls.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/mks.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/msl.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/omx_core.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/omx_img.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/omx_ssl.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/ssl.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/riviera/rvt.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/buzzer.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/datalight.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/die.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/dio_il.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/dma.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs_drv.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs_pcm.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/gbi.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/gil.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/hash.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/kpd.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/lcc.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/lcd_drv_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/mpk.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/power.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/rfs.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/rfsfat.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/rng.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/rtc_drv.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/secure_rom.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/uart_drv.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/uicc.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/usb.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/usbfax.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_core/clkm/drivers_int_ram.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_core/drivers_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/init.a
LINK_OBJ += $(LIBSDIR)/csw-system/nucleus/nucleus_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/nucleus/nucleus_int_ram.a
LINK_OBJ += $(LIBSDIR)/csw-system/rtslib/rts16le_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/rtslib/rts16le_int_ram.a
LINK_OBJ += $(LIBSDIR)/g23m/remu_int_ram.a
LINK_OBJ += $(LIBSDIR)/g23m/config_gprs_fl.a
LINK_OBJ += $(LIBSDIR)/g23m/config_gprs_ir.a
LINK_OBJ += $(LIBSDIR)/g23m/g23m.a
LINK_OBJ += $(LIBSDIR)/gpf/ccddata.a
LINK_OBJ += $(LIBSDIR)/thirdparty/reliance.lib
LINK_OBJ += $(LIBSDIR)/gpf/ccd_na7_db_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/frame_na7_db_fl_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/frame_na7_db_ir_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/misc_na7_db_fl_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/misc_na7_db_ir_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/osx_na7_db_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/remu_na7_db_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/tif_na7_db_fl_ts3.lib
LINK_OBJ += $(LIBSDIR)/gpf/tif_na7_db_ir_ts3.lib
LINK_OBJ += $(LIBSDIR)/layer1/l1_gtt_ext.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_gtt_int.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/tty.a
LINK_OBJ += $(LIBSDIR)/thirdparty/flashfx.lib
LINK_OBJ += $(LIBSDIR)/thirdparty/bae.a
LINK_OBJ += $(LIBSDIR)/thirdparty/bae_sram.a
LINK_OBJ += $(SRCDIR)/vin.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/wheel.a

include $(BASEDIR)/Makefile.rules

SUBDIRS = src

.PHONY: $(SUBDIRS)

all: $(TARGET)

$(TARGET): $(MOTOROLA)
	$(EXTRACT) $(MOTOROLA) $@

$(MOTOROLA): $(TARGETOUT)
	$(HEX) $(HEXFLAGS) -o $@ $(TARGETOUT)

$(TARGETOUT): $(LINKOUT)
	$(MERGE) $(LINKOUT) $(BASEBAND) $@

$(LINKOUT) $(LINKMAP): $(SUBDIRS) $(LINKCMD)
	$(LD) $(LDFLAGS) -o $@ -m $(LINKMAP) $(LINKCMD) $(LINK_OBJ)

$(LINKCMD): $(CMD_MAP) $(CMD_OPT_PP)
	$(PERL) $(CMD_TOOL) $(CMD_OPT_PP) $@ 3 $(CMD_MAP)

$(CMD_OPT_PP): $(LCFSRC)
	$(CC) -ppl $(CFLAGS) $<
	$(MV) $(patsubst %.c,%.pp,$(LCFSRC)) $(CMD_OPT_PP)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)
	$(RM) $(TARGET)
	$(RM) $(MOTOROLA)
	$(RM) $(TARGETOUT)
	$(RM) $(LINKOUT)
	$(RM) $(LINKMAP)
	$(RM) $(LINKCMD)
	$(RM) $(CMD_OPT_PP)
#	$(MAKE) -C src clean
