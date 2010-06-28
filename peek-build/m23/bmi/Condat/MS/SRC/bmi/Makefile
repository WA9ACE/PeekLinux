DEFINE += 
UNDEFINE += RSSI
UNDEFINE += PCM_2_FFS

CCBASE =  -mv4 --abi=tiabi

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
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


SRC  = AUICalcEditor.c
SRC += AUIEditor.c
SRC += AUIPinEditor.c
SRC += AUISymbol.c
SRC += AUITextEntry.c
SRC += MmiAoc.c
SRC += MmiBand.c
SRC += MmiBlkLangDB.c
SRC += MmiBlkManager.c
SRC += MmiBlkResources.c
SRC += MmiBookCallDetailsWindow.c
SRC += MmiBookCallListWindow.c
SRC += MmiBookController.c
SRC += MmiBookDetailsWindow.c
SRC += MmiBookMenuWindow.c
SRC += MmiBookPhonebook.c
SRC += MmiBookSDNWindow.c
SRC += MmiBookSearchWindow.c
SRC += MmiBookServices.c
SRC += MmiBookShared.c
SRC += MmiBookUtils.c
SRC += MmiCPHS.c
SRC += MmiCalculatorMain.c
SRC += mmiCall.c              
SRC += MmiChineseInput.c
SRC += MmiDialogs.c
SRC += mmiDummy.c
SRC += mmiEm.c
SRC += MmiIcons.c
SRC += Mmigame.c
SRC += MmiGprs.c
SRC += MmiHomezone.c
SRC += MmiIdle.c
SRC += mmiFileManager.c
SRC += MmiLists.c
SRC += MmiMain.c
SRC += MmiMelody_data.c
SRC += MmiMenu.c
SRC += MmiMidiTest.c
SRC += MmiMmi.c
SRC += MmiMP3Test.c
SRC += MmiNetwork.c
SRC += MmiPins.c
SRC += MmiResources.c
SRC += mmiSatClassE.c
SRC += mmiSatCall.c
SRC += mmiSatInfo.c
SRC += mmiSatInput.c
SRC += mmiSatMenu.c
SRC += MmiServices.c
SRC += MmiSettings.c
SRC += MmiSimToolkit.c
SRC += mmiSmsBroadcast.c
SRC += mmiSmsIdle.c
SRC += mmiSmsMenu.c
SRC += mmiSmsRead.c
SRC += mmismssend.c
SRC += MmiSoftKeys.c
SRC += MmiSounds.c
SRC += MmiStart.c
SRC += MmiTimeDate.c
SRC += MmiTimers.c
SRC += MmiUserData.c
SRC += MmiVoiceMemo.c
SRC += mmiprogressbar.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/l1_gtt_int.a

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
