VARIANT_DEFINE :=

DEFINE += $(EMO_SIM)

INCLUDE  = $(SRCDIR)/netsurf/emoport
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/netdepends/libnsbmp/include
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC = src/libnsbmp.c

DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libnsbmp.a

all: $(TARGET)
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

$(TARGET): $(OBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

