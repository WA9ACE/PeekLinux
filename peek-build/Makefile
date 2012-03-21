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

STR2IND_C  = $(BUILDDIR)/trace/str2ind.c

UNDEFINE += 

SRCSUBDIR  = netdepends/hubbub-0.0.2 
SRCSUBDIR += netdepends/libwapcaplet
SRCSUBDIR += netdepends/libparserutils
SRCSUBDIR += netdepends/libxml2
SRCSUBDIR += netdepends/libnsfb
SRCSUBDIR += netdepends/libnsgif
SRCSUBDIR += netdepends/libnsbmp
SRCSUBDIR += netdepends/libjpeg
SRCSUBDIR += netdepends/libcss
SRCSUBDIR += netdepends/curl-7.21.1
SRCSUBDIR += netsurf
SRCSUBDIR += regex
SRCSUBDIR += zlib
SRCSUBDIR += libpng
SRCSUBDIR += freetype2
SRCSUBDIR += lua
SRCSUBDIR += nmealib
SRCSUBDIR += tasks
SRCSUBDIR += emobiix
SRCSUBDIR += sdl

SRCLIB  = $(SRCDIR)/zlib/libz.a 
SRCLIB += $(SRCDIR)/sdl/libsdl.a
SRCLIB += $(SRCDIR)/freetype2/freetype2.a 
SRCLIB += $(SRCDIR)/libpng/libpng.a 
SRCLIB += $(SRCDIR)/netdepends/libjpeg/libjpeg.a
SRCLIB += $(SRCDIR)/zlib/libz.a 
SRCLIB += $(SRCDIR)/tasks/peitasks.a 
SRCLIB += $(SRCDIR)/lua/lua.a 
SRCLIB += $(SRCDIR)/nmealib/nmealib.a 
SRCLIB += $(SRCDIR)/emobiix/emobiix.a 
SRCLIB += $(SRCDIR)/netsurf/libnetsurf.a 
SRCLIB += $(SRCDIR)/regex/regex.a 
SRCLIB += $(SRCDIR)/netdepends/libcss/libcss.a 
SRCLIB += $(SRCDIR)/netdepends/libnsgif/libnsgif.a
SRCLIB += $(SRCDIR)/netdepends/libnsbmp/libnsbmp.a
SRCLIB += $(SRCDIR)/netdepends/curl-7.21.1/libcurl.a
SRCLIB += $(SRCDIR)/netdepends/libnsfb/libnsfb.a 
SRCLIB += $(SRCDIR)/netdepends/libxml2/libxml2.a 
SRCLIB += $(SRCDIR)/netdepends/libparserutils/libparserutils.a 
SRCLIB += $(SRCDIR)/netdepends/hubbub-0.0.2/libhubbub.a 
SRCLIB += $(SRCDIR)/netdepends/libwapcaplet/libwapcaplet.a
SRCLIB += $(SRCDIR)/emobiix/protocol/protocol.a

CMD_MAP  = $(M23DIR)/chipsetsw/system/template/gsm_is_lj3.template 
CMD_MAP += $(LIBSDIR)/layer1/l1_intram.o 
#CMD_MAP += $(M23DIR)/m23libs/l1_intram.o
CMD_MAP += $(M23DIR)/csw-system/drv_core/clkm/drivers_int_ram.a 
CMD_MAP += $(M23DIR)/L1/layer1/tpu_drivers/tpudrv.a 
CMD_MAP += $(LIBSDIR)/layer1/l1_int.a 
#CMD_MAP += $(M23DIR)/m23libs/l1_int.a
CMD_MAP += $(M23DIR)/L1/layer1/cust1/l1_custom_int.a 
#CMD_MAP += $(M23DIR)/m23libs/l1_custom_int.a
CMD_MAP += $(LIBSDIR)/layer1/l1_ext.a 
#CMD_MAP += $(M23DIR)/m23libs/l1_ext.a
CMD_MAP += $(LIBSDIR)/csw-system/init.a 
#CMD_MAP += $(M23DIR)/m23libs/init.a
CMD_MAP += $(M23DIR)/chipsetsw/Services/dar/dar_gbl_var.a 
CMD_MAP += $(M23DIR)/chipsetsw/Services/Audio/audio_int_ram.o
#CMD_MAP += $(M23DIR)/m23libs/audio_int_ram.o
CMD_MAP += $(M23DIR)/CS_ThirdParty/Beatnik/bae_sram.a 
CMD_MAP += $(M23DIR)/CS_ThirdParty/Beatnik/bae_sram.a 
CMD_MAP += $(M23DIR)/CS_ThirdParty/Emuzed/jpeg_intram.a 
CMD_MAP += $(M23DIR)/csw-system/rtslib/v2_70/rts16le_int_ram.a 
CMD_MAP += $(M23DIR)/csw-system/os/nucleus/1_15/plus/src/nucleus_int_ram.a 
CMD_MAP += $(M23DIR)/gpf/LIB/osx_na7_db.lib 
CMD_MAP += $(M23DIR)/gpf/LIB/frame_na7_db_ir.lib 
CMD_MAP += $(M23DIR)/gpf/LIB/misc_na7_db_ir.lib 
CMD_MAP += $(M23DIR)/gpf/LIB/tif_na7_db_ir.lib 
CMD_MAP += $(M23DIR)/g23m-fad/ppp/ppp_ir.a 
CMD_MAP += $(M23DIR)/g23m-gprs/cci/cci_ir.a 
#CMD_MAP += $(M23DIR)/g23m-gprs/grlc/grlc_ir.a 
CMD_MAP += $(LIBSDIR)/g23m-gprs/grlc_ir.a 
CMD_MAP += $(M23DIR)/g23m/condat/frame/config/config_gprs_ir.a
CMD_MAP += $(M23DIR)/g23m/condat/com/src/config/remu_int_ram.a 
CMD_MAP += $(M23DIR)/CS_ThirdParty/Beatnik/bae.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/as.a 
CMD_MAP += $(LIBSDIR)/chipsetsw/services/audio.a 
#CMD_MAP += $(M23DIR)/m23libs/audio.a
CMD_MAP += $(M23DIR)/csw-system/drv_app/rfs/rfs.a 
CMD_MAP += $(M23DIR)/csw-system/rtslib/v2_70/rts16le_int_ram.lib 
CMD_MAP += $(M23DIR)/gpf/LIB/remu_na7_db_ts3.lib 
CMD_MAP += $(M23DIR)/gpf/LIB/frame_na7_db_fl_ts3.lib

STR2IND_OBJ  = $(BUILDDIR)/trace/str2ind.o 

LINK_OBJ  = $(M23DIR)/CS_ThirdParty/Emuzed/jpeg_codec.a 
LINK_OBJ += $(M23DIR)/CS_ThirdParty/Emuzed/jpeg_intram.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_ext.a
#LINK_OBJ += $(M23DIR)/m23libs/l1_ext.a
#LINK_OBJ += $(M23DIR)/m23libs/l1_int.a
#LINK_OBJ += $(M23DIR)/m23libs/l1_intram.o
LINK_OBJ += $(LIBSDIR)/layer1/l1_int.a
LINK_OBJ += $(LIBSDIR)/layer1/l1_intram.o
LINK_OBJ += $(M23DIR)/L1/layer1/cust1/l1_custom_int.a 
LINK_OBJ += $(M23DIR)/L1/layer1/tm_cust0/l1_custom_ext.a
LINK_OBJ += $(M23DIR)/L1/layer1/tpu_drivers/tpudrv.a
LINK_OBJ += $(M23DIR)/bmi/bmi.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/audio.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/audio_bgd.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/Audio/audio_int_ram.o
#LINK_OBJ += $(M23DIR)/m23libs/audio.a
#LINK_OBJ += $(M23DIR)/m23libs/audio_bgd.a
#LINK_OBJ += $(M23DIR)/m23libs/audio_int_ram.o
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/as.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/cst/cst.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/dar/dar.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/dar/dar_gbl_var.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/etm/etm.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/img/img.a
# m18 TODO (flicker display) LINK_OBJ += $(M23DIR)/chipsetsw/Services/lls/lls.a
LINK_OBJ += $(LIBSDIR)/chipsetsw/services/lls.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/mks/mks.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/msl/msl.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/omx_core/omx_core.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/omx_img/omx_img.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/omx_ssl/omx_ssl.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/ssl/ssl.a
LINK_OBJ += $(M23DIR)/chipsetsw/riviera/rvt/rvt.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/buzzer/buzzer.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/datalight/datalight.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/die/die.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/dio_il/dio_il.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/dma/dma.a
#LINK_OBJ += $(M23DIR)/csw-system/drv_app/ffs/ffs.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs_drv.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/ffs_pcm.a
#LINK_OBJ += $(M23DIR)/csw-system/drv_app/ffs/ffs_drv.a
#LINK_OBJ += $(M23DIR)/csw-system/drv_app/ffs/ffs_pcm.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/gbi/gbi.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/gil/gil.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/hash/hash.a
# m18 TODO (code) LINK_OBJ += $(M23DIR)/csw-system/drv_app/kpd/kpd.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/kpd.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/lcc/lcc.a
#LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/lcc.a
# m18 TODO (code) LINK_OBJ += $(M23DIR)/csw-system/drv_app/lcd/lcd_drv_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/lcd_drv_flash.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/mpk/mpk.a
# TODO linking error in conjunction with init.a
#LINK_OBJ += $(M23DIR)/csw-system/drv_app/power/power.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/power.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/rfs/rfs.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/rfsfat/rfsfat.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/rng/rng.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/rtc/rtc_drv.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/secure_rom/secure_rom.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/uart/uart_drv.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/uicc/uicc.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/usb/usb.a
LINK_OBJ += $(M23DIR)/csw-system/drv_app/usbfax/usbfax.a
LINK_OBJ += $(M23DIR)/csw-system/drv_core/clkm/drivers_int_ram.a
# TODO linking error in conjunction with init.a
#LINK_OBJ += $(M23DIR)/csw-system/drv_core/drivers_flash.a
LINK_OBJ += $(LIBSDIR)/csw-system/drv_core/drivers_flash.a
# TODO linking error for asm functions 
# LINK_OBJ += $(M23DIR)/csw-system/init_common/init.a
LINK_OBJ += $(LIBSDIR)/csw-system/init.a
#LINK_OBJ += $(M23DIR)/m23libs/init.a
LINK_OBJ += $(M23DIR)/csw-system/os/nucleus/1_15/plus/src/nucleus_flash.a
LINK_OBJ += $(M23DIR)/csw-system/os/nucleus/1_15/plus/src/nucleus_int_ram.a
LINK_OBJ += $(M23DIR)/csw-system/rtslib/v2_70/rts16le_flash.a
LINK_OBJ += $(M23DIR)/csw-system/rtslib/v2_70/rts16le_int_ram.a
LINK_OBJ += $(M23DIR)/g23m/condat/com/src/config/remu_int_ram.a
LINK_OBJ += $(M23DIR)/g23m/condat/frame/config/config_gprs_fl.a
LINK_OBJ += $(M23DIR)/g23m/condat/frame/config/config_gprs_ir.a
LINK_OBJ += $(M23DIR)/g23m/g23m.a
LINK_OBJ += $(M23DIR)/gpf/CCD/ccddata.a
#LINK_OBJ += $(M23DIR)/CS_ThirdParty/Datalight/libs/reliance.lib
LINK_OBJ += $(LIBSDIR)/thirdparty/reliance.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/ccd_na7_db_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/frame_na7_db_fl_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/frame_na7_db_ir_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/misc_na7_db_fl_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/misc_na7_db_ir_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/osx_na7_db_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/remu_na7_db_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/tif_na7_db_fl_ts3.lib
LINK_OBJ += $(M23DIR)/gpf/LIB/tif_na7_db_ir_ts3.lib
LINK_OBJ += $(M23DIR)/L1/layer1/gtt_cfile/l1_gtt_ext.a
LINK_OBJ += $(M23DIR)/L1/layer1/gtt_cfile/l1_gtt_int.a
LINK_OBJ += $(M23DIR)/chipsetsw/Services/tty/tty.a
#LINK_OBJ += $(M23DIR)/CS_ThirdParty/Datalight/libs/flashfx.lib
LINK_OBJ += $(LIBSDIR)/thirdparty/flashfx.lib
LINK_OBJ += $(M23DIR)/CS_ThirdParty/Beatnik/bae.a
LINK_OBJ += $(M23DIR)/CS_ThirdParty/Beatnik/bae_sram.a
LINK_OBJ += $(SRCLIB)
LINK_OBJ += $(LIBSDIR)/csw-system/drv_app/wheel.a

include $(BASEDIR)/Makefile.rules

SUBDIRS = $(addprefix src/,$(SRCSUBDIR)) m23

.PHONY: $(SUBDIRS)

all: $(TARGET)

$(TARGET): $(MOTOROLA)
	$(EXTRACT) $(MOTOROLA) $@

$(MOTOROLA): $(TARGETOUT)
	$(HEX) $(HEXFLAGS) -o $@ $(TARGETOUT)

$(TARGETOUT): $(LINKOUT)
	$(MERGE) $(LINKOUT) $(BASEBAND) $@

$(LINKOUT) $(LINKMAP): $(SUBDIRS) $(LINKCMD) $(STR2IND_OBJ)
	$(LD) $(LDFLAGS) -o $@ -m $(LINKMAP) $(LINKCMD) $(STR2IND_OBJ) $(LINK_OBJ)

$(LINKCMD): $(CMD_OPT_PP) $(SUBDIRS)
	$(PERL) $(CMD_TOOL) $(CMD_OPT_PP) $@ 3 $(CMD_MAP)

$(CMD_OPT_PP): $(LCFSRC)
	$(CC) -ppl $(CFLAGS) $<
	$(MV) $(patsubst %.c,%.pp,$(LCFSRC)) $(CMD_OPT_PP)

$(STR2IND_OBJ): $(STR2IND_C)
	$(CC) $(CFLAGS_BASE) -eoo -fr $(@D) $<

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

clean: $(SUBDIRS)
	$(RM) $(STR2IND_OBJ)
	$(RM) $(TARGET)
	$(RM) $(MOTOROLA)
	$(RM) $(TARGETOUT)
	$(RM) $(LINKOUT)
	$(RM) $(LINKMAP)
	$(RM) $(LINKCMD)
	$(RM) $(CMD_OPT_PP)
