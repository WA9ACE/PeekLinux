DEFINE += SIMULATOR
DEFINE += LIBPNG_EXPORTS
DEFINE += LINUX

PNGDIR = $(SRCDIR)/libpng

INCLUDE  = $(PNGDIR)

SRC  = png.c
SRC += pngerror.c
SRC += pngget.c
SRC += pngmem.c
SRC += pngpread.c
SRC += pngread.c
SRC += pngrio.c
SRC += pngrtran.c
SRC += pngrutil.c
SRC += pngset.c
SRC += pngtrans.c
SRC += pngwio.c
SRC += pngwrite.c
SRC += pngwtran.c
SRC += pngwutil.c

OBJ = $(addprefix $(PNGDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libpng.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules
