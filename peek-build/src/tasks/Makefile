DEFINE +=

INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/csw-system/drv_core/armio
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15/plus
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/res
INCLUDE += $(SRCDIR)/baf
INCLUDE += $(SRCDIR)/bui
INCLUDE += $(SRCDIR)/bui/uiex
INCLUDE += $(SRCDIR)/bam
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/tasks
INCLUDE += $(SRCDIR)/tasks/hwtask
INCLUDE += $(SRCDIR)/nmealib/include
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/csw-system/drv_app/kpd
INCLUDE += $(M23DIR)/g23m-gsm/sim
INCLUDE += $(M23DIR)/csw-system/drv_app/ffs
INCLUDE += $(M23DIR)/csw-system/drv_core/abb
INCLUDE += $(M23DIR)/csw-system/drv_core/i2c
INCLUDE += $(M23DIR)/csw-system/drv_core/util

#SRC = emo_pei.c
SRC  = ui_pei.c
#SRC += trans_pei.c

SRC += hwtask/at.c
SRC += hwtask/hwevents.c
SRC += hwtask/timer.c
SRC += hwtask/hwpei.c
SRC += hwtask/hwtask.c
SRC += hwtask/kbd.c
SRC += hwtask/mmi.c
SRC += hwtask/acie.c
SRC += hwtask/tmr.c
SRC += hwtask/sim.c
SRC += hwtask/time.c
SRC += hwtask/gprs.c
SRC += hwtask/backlight.c
SRC += hwtask/stub.c
SRC += hwtask/vibrator.c


BALOBJ  = hwtask/bal/kpd_drv_if.o
BALOBJ += hwtask/bal/balkeypad.o
BALOBJ += hwtask/bal/balmisc.o
BALOBJ += hwtask/bal/bal_ffs.o
BALOBJ += hwtask/bal/balimeiapi.o

TARGET = $(CURDIR)/peitasks.a
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

$(TARGET): $(OBJ) $(BALOBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
