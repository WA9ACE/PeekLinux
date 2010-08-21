SUBDIR_DEPEND = $(SRCDIR)/zlib/libz.a \
		$(SRCDIR)/freetype2/freetype2.a \
		$(SRCDIR)/libpng/libpng.a \
		$(SRCDIR)/zlib/libz.a \
		$(SRCDIR)/tasks/peitasks.a \
		$(SRCDIR)/lua/lua.a \
		$(SRCDIR)/nmealib/nmealib.a \
		$(SRCDIR)/emobiix/emobiix.a \
		$(SRCDIR)/netsurf/libnetsurf.a \
		$(SRCDIR)/regex/regex.a \
		$(SRCDIR)/netdepends/libcss/libcss.a \
		$(SRCDIR)/netdepends/libnsfb/libnsfb.a \
		$(SRCDIR)/netdepends/libxml2/libxml2.a \
		$(SRCDIR)/netdepends/libparserutils/libparserutils.a \
		$(SRCDIR)/emobiix/protocol/protocol.a

TARGET = $(SRCDIR)/vin.a

SUBDIR = netdepends/libparserutils netdepends/libxml2 netdepends/libnsfb netdepends/libcss netsurf regex zlib libpng freetype2 lua nmealib tasks emobiix

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
	$(MAKE) -C emobiix clean
	$(MAKE) -C tasks clean
	$(MAKE) -C regex clean
	$(MAKE) -C netsurf clean
	$(MAKE) -C libxml2 clean

include $(BASEDIR)/Makefile.rules
