DEFINE += FT2_BUILD_LIBRARY
DEFINE += FT_CONFIG_MODULES_H="<ftmodule.h>"
DEFINE += FT_Message=emo_printf
DEFINE += FT_Panic=emo_printf
DEFINE += SIMULATOR

FTDIR = $(SRCDIR)/freetype2

INCLUDE = $(FTDIR)/include

SRC += src/autofit/afangles.c
SRC += src/autofit/afcjk.c
SRC += src/autofit/afdummy.c
SRC += src/autofit/afglobal.c
SRC += src/autofit/afhints.c
SRC += src/autofit/afindic.c
SRC += src/autofit/aflatin.c
SRC += src/autofit/afloader.c
SRC += src/autofit/afmodule.c
SRC += src/autofit/afpic.c
SRC += src/autofit/afwarp.c
SRC += src/base/basepic.c
SRC += src/base/ftadvanc.c
SRC += src/base/ftapi.c
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
SRC += src/base/ftobjs.c
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
SRC += src/psnames/psmodule.c
SRC += src/psnames/pspic.c
SRC += src/sfnt/sfdriver.c
SRC += src/sfnt/sfntpic.c
SRC += src/sfnt/sfobjs.c
SRC += src/sfnt/ttbdf.c
SRC += src/sfnt/ttcmap.c
SRC += src/sfnt/ttkern.c
SRC += src/sfnt/ttload.c
SRC += src/sfnt/ttmtx.c
SRC += src/sfnt/ttpost.c
SRC += src/sfnt/ttsbit.c
SRC += src/smooth/ftgrays.c
SRC += src/smooth/ftsmooth.c
SRC += src/smooth/ftspic.c
SRC += src/truetype/ttdriver.c
SRC += src/truetype/ttgload.c
SRC += src/truetype/ttgxvar.c
SRC += src/truetype/ttinterp.c
SRC += src/truetype/ttobjs.c
SRC += src/truetype/ttpic.c
SRC += src/truetype/ttpload.c

OBJ = $(addprefix $(FTDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/freetype2.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules
