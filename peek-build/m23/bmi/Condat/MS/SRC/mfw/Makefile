
DEFINE +=
DEFINE += EMO_DBG
DEFINE += TRACE_FUNCTION=emo_printf
DEFINE += TRACE_ERROR=emo_printf
DEFINE += TRACE_EVENT=emo_printf
DEFINE += TRACE_EVENT_P1=emo_printf
DEFINE += TRACE_EVENT_P2=emo_printf
DEFINE += TRACE_EVENT_P3=emo_printf
DEFINE += TRACE_EVENT_P4=emo_printf
DEFINE += TRACE_EVENT_P5=emo_printf
DEFINE += TRACE_EVENT_P7=emo_printf
DEFINE += TRACE_FUNCTION_P1=emo_printf
DEFINE += TRACE_FUNCTION_P2=emo_printf


UNDEFINE += RSSI
UNDEFINE += PCM_2_FFS

CCSTRICT = 
CCBASE = -mv4 --abi=tiabi

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/bmi/ZI/include
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Engine/Client_API/Private
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Engine/Client_API/Public
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Engine/Common
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Engine/Common_FLASH
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Libraries/G711_Source
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/Libraries/zlib_Source
INCLUDE += $(M23DIR)/CS_ThirdParty/Beatnik/code/services/bae/PAL_Impls/Riviera
INCLUDE += $(M23DIR)/csw-system/drv_app/kpd
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/mfw
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/atb
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/bmi/Icons
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/atb
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m/condat/com
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/ksd
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/g23m-aci/aci_ext
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/bmi/ZI/data
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Code/Inc

SRC  = Mfw_band.c
SRC += mfw_version.c
#SRC  += mfw_BtipsA2dp.c
#SRC  += mfw_BtipsMdg.c
#SRC  += mfw_BtipsVg.c
#SRC  += mfw_bt_api.c
SRC  += mfw_cm.c
SRC  += mfw_ext.c
SRC  += mfw_gprs.c
SRC  += mfw_lng.c
SRC  += mfw_mme.c
SRC  += mfw_nm.c
SRC  += mfw_sat.c
SRC  += mfw_sms.c
SRC  += mfw_td.c
SRC  += mfw_win.c
SRC  += Mfw_em.c
#SRC  += mfw_BtipsBmg.c
#SRC  += mfw_BtipsOppc.c
SRC  += mfw_acie.c
#SRC  += mfw_camapp.c
SRC  += mfw_cnvt.c
SRC  += mfw_ffs.c
SRC  += mfw_icn.c
SRC  += mfw_mfw.c
SRC  += mfw_mnu.c
SRC  += mfw_nma.c
SRC  += mfw_sim.c
SRC  += mfw_ss.c
SRC  += mfw_tim.c
SRC  += ti1_key.c
#SRC  += mfw_Btips.c
#SRC  += mfw_BtipsFtps.c
#SRC  += mfw_BtipsOpps.c
SRC  += mfw_aud.c
SRC  += mfw_cb.c
SRC  += mfw_cphs.c
SRC  += mfw_fm.c
SRC  += mfw_kbd.c
SRC  += mfw_midi.c
SRC  += mfw_mp3test.c
SRC  += mfw_phb.c
SRC  += mfw_sima.c
SRC  += mfw_str.c
SRC  += mfw_utils.c
SRC  += ti1_tmr.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/bmi-mfw.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
