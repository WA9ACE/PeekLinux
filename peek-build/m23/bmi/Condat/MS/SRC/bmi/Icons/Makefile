DEFINE += 
UNDEFINE += RSSI
UNDEFINE += PCM_2_FFS

CCBASE =  -mv4 --abi=tiabi

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
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/mfw
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/atb
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/bmi/Icons
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m/condat/com
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/ksd
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/cdginc


SRC = IcnBattery.c
SRC += IcnBgdSquares.c
SRC += IcnCallBackGround.c
SRC += IcnCipher.c
SRC += IcnDefs.c
SRC += IcnForward.c
SRC += IcnGprs.c
SRC += IcnHomezone.c
SRC += IcnKeylock.c
SRC += IcnLogoTI.c
SRC += IcnMainApp.c
SRC += IcnMainDiv.c
SRC += IcnMainMsg.c
SRC += IcnMainPhb.c
SRC += IcnMainRcl.c
SRC += IcnMainSec.c
SRC += IcnMainSet.c
SRC += IcnMainStk.c
SRC += IcnMainWap.c
SRC += IcnMessage.c
SRC += IcnNetBouygues.c
SRC += IcnNetChinamobile.c
SRC += IcnNetFareast.c
SRC += IcnNetSFR.c
SRC += IcnNetTMobile.c
SRC += IcnNetVodafone.c
SRC += IcnNetVodafoneD2.c
SRC += IcnNetorange.c
SRC += IcnNetsonofon.c
SRC += IcnPopupBgd.c
SRC += IcnSignal.c
SRC += IcnVoicemail.c
SRC += icnAniPhone.c
SRC += icnBgdTI.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/bmi-Icons.a

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
