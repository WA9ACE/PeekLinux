DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/g23m-aci/aci/aci.a \
					$(M23DIR)/g23m-gprs/grlc/grlc.a \
					$(M23DIR)/g23m-gsm/rr/rr.a \
					$(M23DIR)/g23m-gsm/cc/cc.a \
					$(M23DIR)/g23m-gsm/dl/dl.a \
					$(M23DIR)/g23m-gsm/ss/ss.a \
					$(M23DIR)/g23m-gsm/alr/alr.a \
					$(M23DIR)/g23m-gsm/l1/l1_pei.a \
					$(M23DIR)/g23m-gsm/mm/mm.a \
					$(M23DIR)/g23m-gsm/sms/sms.a \
					$(M23DIR)/g23m-gsm/sim/sim.a

SUBDIR =	$(M23DIR)/g23m-aci/aci \
					$(M23DIR)/g23m-gprs/grlc \
					$(M23DIR)/g23m-gsm/sim \
					$(M23DIR)/g23m-gsm/cc \
					$(M23DIR)/g23m-gsm/dl \
					$(M23DIR)/g23m-gsm/ss \
					$(M23DIR)/g23m-gsm/alr \
					$(M23DIR)/g23m-gsm/l1 \
					$(M23DIR)/g23m-gsm/mm \
					$(M23DIR)/g23m-gsm/sms \
					$(M23DIR)/g23m-gsm/rr

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
