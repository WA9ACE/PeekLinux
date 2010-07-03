DEFINE += VIN_DEBUG
DEFINE += BGET
DEFINE += BAL_IP_PUSH

#VIN_DEPEND = $(SRCDIR)/bal/bal.a \
#             $(SRCDIR)/bal/gprs.a \
#	     $(SRCDIR)/bal/ata.a  \
#             $(SRCDIR)/bal/bget.a
VIN_DEPEND = $(SRCDIR)/bal/bget.a \
	     $(SRCDIR)/bal/bal.a

SUBDIR_DEPEND = $(SRCDIR)/zlib/libz.a
#$(SRCDIR)/freetype2/freetype2.a \
		$(SRCDIR)/libpng/libpng.a \
#$(SRCDIR)/zlib/libz.a \
#                $(SRCDIR)/bam/bam.a \
#                $(SRCDIR)/baf/baf.a \
#		$(SRCDIR)/lua/lua.a \
#		$(SRCDIR)/nmealib/nmealib.a \
#                $(SRCDIR)/emobiix/emobiix.a \
#                $(SRCDIR)/emobiix/protocol/protocol.a \
#                $(SRCDIR)/bui/bui.a

ifeq (,$(findstring EMO_SIM,$(VARIANT_DEFINE)))
	VIN_DEPEND += $(SRCDIR)/bal/socket.a
else
SUBDIR_DEPEND += $(SRCDIR)/emobiix/socket_sim.a
endif

TARGET = $(SRCDIR)/vin.a

SUBDIR = zlib libpng freetype2 lua nmealib bam baf emobiix bui

.PHONY: $(VIN_DEPEND) $(SUBDIR)

all: $(TARGET)

$(TARGET): $(VIN_DEPEND) $(SUBDIR_DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(VIN_DEPEND): 
	$(AR) -xv $@

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
