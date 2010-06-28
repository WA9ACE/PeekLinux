DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/g23m-aci/aci/aci.a \
					$(M23DIR)/g23m-gsm/rr/rr.a \
					$(M23DIR)/g23m-gsm/cc/cc.a \
					$(M23DIR)/g23m-gsm/dl/dl.a \
					$(M23DIR)/g23m-gsm/ss/ss.a \
					$(M23DIR)/g23m-gsm/alr/alr.a \
					$(M23DIR)/g23m-gsm/l1/l1_pei.a \
					$(M23DIR)/g23m-gsm/mm/mm.a \
					$(M23DIR)/g23m-gsm/sms/sms.a \
					$(M23DIR)/g23m-gsm/sim/sim.a \
					$(M23DIR)/g23m-gprs/cci/cci.a \
					$(M23DIR)/g23m-gprs/cl/cl.a \
					$(M23DIR)/g23m-gprs/sm/sm.a \
					$(M23DIR)/g23m-gprs/grlc/grlc.a \
					$(M23DIR)/g23m-gprs/llc/llc.a \
					$(M23DIR)/g23m-gprs/gmm/gmm.a \
					$(M23DIR)/g23m-gprs/grr/grr.a \
					$(M23DIR)/g23m-gprs/sndcp/sndcp.a \
					$(M23DIR)/g23m-fad/fad/fad.a \
					$(M23DIR)/g23m-fad/ra/ra.a \
					$(M23DIR)/g23m-fad/rlp/rlp.a \
					$(M23DIR)/g23m-fad/t30/t30.a \
					$(M23DIR)/g23m-fad/l2r/l2r.a \
					$(M23DIR)/g23m-fad/app/app.a \
					$(M23DIR)/g23m-fad/tcpip/tcpip.a \
					$(M23DIR)/g23m-fad/tcpip/socket_lib.a \
					$(M23DIR)/g23m-fad/ip/ip.a \
					$(M23DIR)/g23m-fad/udp/udp.a \
					$(M23DIR)/g23m-fad/ppp/ppp.a 

TARGET = $(CURDIR)/g23m.a

.PHONY: $(DEPEND) subdir

all: $(TARGET)

$(TARGET): $(DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(DEPEND): subdir
	$(AR) -xv $@

subdir:
	$(MAKE) -C $(M23DIR)/g23m-aci/aci 
	$(MAKE) -C $(M23DIR)/g23m-gsm/sim 
	$(MAKE) -C $(M23DIR)/g23m-gsm/cc 
	$(MAKE) -C $(M23DIR)/g23m-gsm/dl 
	$(MAKE) -C $(M23DIR)/g23m-gsm/ss 
	$(MAKE) -C $(M23DIR)/g23m-gsm/alr 
	$(MAKE) -C $(M23DIR)/g23m-gsm/l1 
	$(MAKE) -C $(M23DIR)/g23m-gsm/mm 
	$(MAKE) -C $(M23DIR)/g23m-gsm/sms 
	$(MAKE) -C $(M23DIR)/g23m-gsm/rr 
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci
	$(MAKE) -C $(M23DIR)/g23m-gprs/cl 
	$(MAKE) -C $(M23DIR)/g23m-gprs/sm 
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc 
	$(MAKE) -C $(M23DIR)/g23m-gprs/llc 
	$(MAKE) -C $(M23DIR)/g23m-gprs/gmm 
	$(MAKE) -C $(M23DIR)/g23m-gprs/grr 
	$(MAKE) -C $(M23DIR)/g23m-gprs/sndcp 
	$(MAKE) -C $(M23DIR)/g23m-fad/fad 
	$(MAKE) -C $(M23DIR)/g23m-fad/ra 
	$(MAKE) -C $(M23DIR)/g23m-fad/rlp
	$(MAKE) -C $(M23DIR)/g23m-fad/t30
	$(MAKE) -C $(M23DIR)/g23m-fad/l2r
	$(MAKE) -C $(M23DIR)/g23m-fad/app
	$(MAKE) -C $(M23DIR)/g23m-fad/tcpip -f Makefile_tcpip
	$(MAKE) -C $(M23DIR)/g23m-fad/tcpip -f Makefile_socket_lib
	$(MAKE) -C $(M23DIR)/g23m-fad/ip 
	$(MAKE) -C $(M23DIR)/g23m-fad/udp 
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp 

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C ../g23m-aci/aci clean

include $(BASEDIR)/Makefile.rules

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
