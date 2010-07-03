DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/g23m-gsm/rr/rr.a \
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
					$(M23DIR)/g23m-fad/ppp/ppp.a \
					$(M23DIR)/g23m-aci/aci/aci.a \
					$(M23DIR)/g23m-aci/aci_ext/aciext.a \
					$(M23DIR)/g23m-aci/aci_dti_mng/aci_dti_mng.a \
					$(M23DIR)/g23m-aci/dti/dti.a \
					$(M23DIR)/g23m-aci/uart/uart_b_lib.a \
					$(M23DIR)/g23m-aci/ksd/ksd.a \
					$(M23DIR)/g23m-aci/bat/bat.a \
					$(M23DIR)/g23m-aci/gdd_dio/gdd_dio.a \
					$(M23DIR)/g23m-aci/l2p/l2p.a \
					$(M23DIR)/g23m/condat/ms/src/ati_ext/atiext.a \
					$(M23DIR)/g23m/condat/com/src/comlib/comlib.a \
					$(M23DIR)/g23m/condat/ms/src/psi/psi.a \
					$(M23DIR)/g23m/condat/com/src/driver/driver.a \
					$(M23DIR)/g23m/condat/ms/src/upm/upm.a \
					$(M23DIR)/g23m/nexgen/coreip/nexgen.a 

TARGET = $(CURDIR)/g23m.a

.PHONY: $(DEPEND) subdir

all: $(TARGET)

$(TARGET): $(DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(DEPEND): subdir
	$(AR) -xv $@

subdir:
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
	$(MAKE) -C $(M23DIR)/g23m-aci/aci
	$(MAKE) -C $(M23DIR)/g23m-aci/aci_ext
	$(MAKE) -C $(M23DIR)/g23m-aci/aci_dti_mng
	$(MAKE) -C $(M23DIR)/g23m-aci/dti
	$(MAKE) -C $(M23DIR)/g23m-aci/uart
	$(MAKE) -C $(M23DIR)/g23m-aci/ksd
	$(MAKE) -C $(M23DIR)/g23m-aci/bat
	$(MAKE) -C $(M23DIR)/g23m-aci/gdd_dio
	$(MAKE) -C $(M23DIR)/g23m-aci/l2p
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/ati_ext
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/config
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/comlib
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/psi
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/driver
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/upm
	$(MAKE) -C $(M23DIR)/g23m/nexgen/coreip

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C $(M23DIR)/g23m-gsm/sim clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/cc clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/dl clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/ss clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/alr clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/l1 clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/mm clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/sms clean
	$(MAKE) -C $(M23DIR)/g23m-gsm/rr clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/cl clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/sm clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/llc clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/gmm clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/grr clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/sndcp clean
	$(MAKE) -C $(M23DIR)/g23m-fad/fad clean
	$(MAKE) -C $(M23DIR)/g23m-fad/ra clean
	$(MAKE) -C $(M23DIR)/g23m-fad/rlp clean
	$(MAKE) -C $(M23DIR)/g23m-fad/t30 clean
	$(MAKE) -C $(M23DIR)/g23m-fad/l2r clean
	$(MAKE) -C $(M23DIR)/g23m-fad/app clean
	$(MAKE) -C $(M23DIR)/g23m-fad/tcpip -f Makefile_tcpip clean
	$(MAKE) -C $(M23DIR)/g23m-fad/tcpip -f Makefile_socket_lib clean
	$(MAKE) -C $(M23DIR)/g23m-fad/ip clean
	$(MAKE) -C $(M23DIR)/g23m-fad/udp clean
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp clean
	$(MAKE) -C $(M23DIR)/g23m-aci/aci clean
	$(MAKE) -C $(M23DIR)/g23m-aci/aci_ext clean
	$(MAKE) -C $(M23DIR)/g23m-aci/aci_dti_mng clean
	$(MAKE) -C $(M23DIR)/g23m-aci/dti clean
	$(MAKE) -C $(M23DIR)/g23m-aci/uart clean
	$(MAKE) -C $(M23DIR)/g23m-aci/ksd clean
	$(MAKE) -C $(M23DIR)/g23m-aci/bat clean
	$(MAKE) -C $(M23DIR)/g23m-aci/gdd_dio clean
	$(MAKE) -C $(M23DIR)/g23m-aci/l2p clean
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/ati_ext clean
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/config clean
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/comlib clean
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/psi clean
	$(MAKE) -C $(M23DIR)/g23m/condat/com/src/driver clean
	$(MAKE) -C $(M23DIR)/g23m/condat/ms/src/upm clean
	$(MAKE) -C $(M23DIR)/g23m/nexgen/coreip clean

include $(BASEDIR)/Makefile.rules
