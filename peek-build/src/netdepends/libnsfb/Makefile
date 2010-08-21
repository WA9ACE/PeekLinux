VARIANT_DEFINE :=

DEFINE = 

INCLUDE  = $(SRCDIR)/netsurf/emoport
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/netdepends/libnsfb/include

SRC  = src/libnsfb.c
SRC += src/cursor.c
SRC += src/plot/api.c
SRC += src/plot/util.c
SRC += src/plot/generic.c
SRC += src/plot/32bpp.c
SRC += src/plot/16bpp.c
SRC += src/plot/8bpp.c
SRC += src/surface/surface.c
SRC += src/surface/peek.c

DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libnsfb.a

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

