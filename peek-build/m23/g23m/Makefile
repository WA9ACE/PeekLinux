DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/g23m-aci/aci/aci.a \
					#$(M23DIR)/g23m-gprs/grlc/grlc.a

SUBDIR =	$(M23DIR)/g23m-aci/aci \
					#$(M23DIR)/g23m-gprs/grlc

TARGET = $(CURDIR)/g23m.a

.PHONY: $(DEPEND) $(SUBDIR)

all: $(TARGET)

$(TARGET): $(DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(DEPEND): $(SUBDIR)
	$(AR) -xv $@

$(SUBDIR):
	$(MAKE) -C $@

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C ../g23m-aci/aci clean

include $(BASEDIR)/Makefile.rules
