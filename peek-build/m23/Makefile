DEFINE += 

all: m23

m23: 
	$(MAKE) -C $(M23DIR)/csw-system/drv_core/clkm $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/buzzer $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/datalight $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/die $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/dio_il $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/dma $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/gbi $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/gil $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/hash $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/kpd $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/lcc $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/mpk $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/rfs $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/rfsfat $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/rng $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/rtc $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/power $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/secure_rom $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/uicc $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/usb $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/csw-system/drv_app/usbfax $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/cfile -f Makefile_l1_ext $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/cfile -f Makefile_l1_int $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/tpu_drivers $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/cust1 $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/gtt_cfile -f Makefile_l1_gtt_ext $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/gtt_cfile -f Makefile_l1_gtt_int $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/L1/layer1/tm_cust0 $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/dar -f Makefile_dar $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/dar -f Makefile_dar_gbl_var $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/Audio -f Makefile_audio_int_ram $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/as $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/cst $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/etm $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/img $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/chipsetsw/Services/lls $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/bmi $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/g23m $(MAKECMDGOALS)
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi $(MAKECMDGOALS)

clean: all

include $(BASEDIR)/Makefile.rules
