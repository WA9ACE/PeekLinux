DEFINE += FT2_BUILD_LIBRARY
DEFINE += FT_CONFIG_MODULES_H="<ftmodule.h>"

INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/bal
INCLUDE += $(SRCDIR)/freetype2/include

SRC  = src/base/basepic.c
SRC += src/base/ftadvanc.c
SRC += src/base/ftapi.c
SRC += src/base/ftbase.c
SRC += src/base/ftbbox.c
SRC += src/base/ftbdf.c
SRC += src/base/ftbitmap.c
SRC += src/base/ftcalc.c
SRC += src/base/ftcid.c
SRC += src/base/ftdbgmem.c
SRC += src/base/ftdebug.c
SRC += src/base/ftfstype.c
SRC += src/base/ftgasp.c
SRC += src/base/ftgloadr.c
SRC += src/base/ftglyph.c
SRC += src/base/ftgxval.c
SRC += src/base/ftinit.c
SRC += src/base/ftlcdfil.c
SRC += src/base/ftmm.c
SRC += src/base/ftotval.c
SRC += src/base/ftoutln.c
SRC += src/base/ftpatent.c
SRC += src/base/ftpfr.c
SRC += src/base/ftpic.c
SRC += src/base/ftrfork.c
SRC += src/base/ftsnames.c
SRC += src/base/ftstream.c
SRC += src/base/ftstroke.c
SRC += src/base/ftsynth.c
SRC += src/base/ftsystem.c
SRC += src/base/fttrigon.c
SRC += src/base/fttype1.c
SRC += src/base/ftutil.c
SRC += src/base/ftwinfnt.c
SRC += src/base/ftxf86.c
SRC += src/truetype/truetype.c
SRC += src/truetype/ttdriver.c
SRC += src/truetype/ttgload.c
SRC += src/truetype/ttgxvar.c
SRC += src/truetype/ttinterp.c
SRC += src/truetype/ttobjs.c
SRC += src/truetype/ttpic.c
SRC += src/truetype/ttpload.c
SRC += src/sfnt/sfdriver.c
SRC += src/sfnt/sfnt.c
SRC += src/sfnt/sfntpic.c
SRC += src/sfnt/sfobjs.c
SRC += src/sfnt/ttbdf.c
SRC += src/sfnt/ttcmap.c
SRC += src/sfnt/ttkern.c
SRC += src/sfnt/ttload.c
SRC += src/sfnt/ttmtx.c
SRC += src/sfnt/ttpost.c
SRC += src/sfnt/ttsbit.c
SRC += src/psnames/psmodule.c
SRC += src/psnames/psnames.c
SRC += src/psnames/pspic.c
SRC += src/smooth/smooth.c
SRC += src/autofit/autofit.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/freetype2.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
