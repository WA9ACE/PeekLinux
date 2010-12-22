DEFINE += SIMULATOR
DEFINE += LINUX

INCLUDE  = $(CURDIR)

SRC  = adler32.c
SRC += compress.c
SRC += crc32.c
SRC += deflate.c
SRC += gzclose.c
SRC += gzlib.c
SRC += gzread.c
SRC += gzwrite.c
SRC += infback.c
SRC += inffast.c
SRC += inflate.c
SRC += inftrees.c
SRC += trees.c
SRC += uncompr.c
SRC += zutil.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libzlib.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules
