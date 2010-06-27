DEFINE += 
UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = board/bspUicc.c
SRC += board/bspUicc_Lnk.c
SRC += board/bspUicc_LnkSM.c
SRC += board/bspUicc_LnkSM_handlers.c
SRC += board/bspUicc_Lnk_dataConfig.c
SRC += board/bspUicc_Lnk_resetMgr.c
SRC += board/bspUicc_Lnk_t0.c
SRC += board/bspUicc_Lnk_t1.c
SRC += board/bspUicc_Phy.c
SRC += board/bspUicc_Phy_intC.c
SRC += board/bspUicc_Power.c
SRC += board/usim_pwr.c
SRC += board/uicc_wrapper.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/uicc.a

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
