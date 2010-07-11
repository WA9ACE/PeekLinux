SUBDIR_DEPEND = $(SRCDIR)/zlib/libz.a \
		$(SRCDIR)/freetype2/freetype2.a \
		$(SRCDIR)/libpng/libpng.a \
		$(SRCDIR)/zlib/libz.a \
		$(SRCDIR)/bam/bam.a \
		$(SRCDIR)/baf/baf.a \
		$(SRCDIR)/lua/lua.a \
		$(SRCDIR)/nmealib/nmealib.a \
		$(SRCDIR)/emobiix/emobiix.a \
		$(SRCDIR)/emobiix/protocol/protocol.a \
		$(SRCDIR)/bal/socket.a \
		$(SRCDIR)/bal/balexe/balexe.a

TARGET = $(SRCDIR)/vin.a

SUBDIR = zlib libpng freetype2 lua nmealib bam baf emobiix bui

.PHONY: $(SUBDIR)

all: $(TARGET)

$(TARGET): $(SUBDIR_DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(SUBDIR_DEPEND): $(SUBDIR)
	$(AR) -xv $@

$(SUBDIR):
	$(MAKE) -C $@

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C lua clean
	$(MAKE) -C nmealib clean
	$(MAKE) -C freetype2 clean
	$(MAKE) -C libpng clean
	$(MAKE) -C zlib clean
	$(MAKE) -C bui clean
	$(MAKE) -C emobiix clean
	$(MAKE) -C baf clean
	$(MAKE) -C bam clean

include $(BASEDIR)/Makefile.rules
