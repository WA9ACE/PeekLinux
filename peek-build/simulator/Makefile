include Makefile.globals

TARGET=simulator
SUBDIR=libemobiix freetype2 lua libpng libprotocol

INCLUDE  = $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/lua
INCLUDE += $(SRCDIR)/freetype2
INCLUDE += $(SRCDIR)/libpng
INCLUDE += $(SRCDIR)/emobiix/server/protocol
INCLUDE += $(SRCDIR)/emobiix/protocol/

DEFINE  = LINUX
DEFINE += SIMULATOR

OBJ = main.o p_malloc.o platform.o

LIB  = libemobiix/libemobiix.a
LIB += freetype2/freetype2.a
LIB += lua/lua.a
LIB += libpng/libpng.a
LIB += libprotocol/libprotocol.a
LIB += -lz
LIB += /usr/lib/libglut.a
LIB += -lGL
LIB += -lpthread
LIB += -lSDL

.PHONY: $(SUBDIR) 

all: $(TARGET)

simulator: $(SUBDIR) $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIB)

libemobiix:
	$(MAKE) -C $@

freetype2:
	$(MAKE) -C $@

lua:
	$(MAKE) -C $@

libpng:
	$(MAKE) -C $@

libprotocol:
	$(MAKE) -C $@

clean: 
	$(MAKE) -C libemobiix clean
	$(MAKE) -C freetype2 clean
	$(MAKE) -C lua clean
	$(MAKE) -C libpng clean
	$(MAKE) -C libprotocol clean
	$(RM) $(TARGET) *.o *.d

include $(BASEDIR)/Makefile.rules
