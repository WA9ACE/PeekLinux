DEFINE += 
UNDEFINE +=

DEPEND = 
DEPEND +=	$(M23DIR)/g23m-gsm/rr/rr.a
DEPEND +=	$(M23DIR)/g23m-gsm/cc/cc.a
DEPEND +=	$(M23DIR)/g23m-gsm/dl/dl.a
DEPEND +=	$(M23DIR)/g23m-gsm/ss/ss.a
DEPEND +=	$(M23DIR)/g23m-gsm/alr/alr.a
DEPEND +=	$(M23DIR)/g23m-gsm/l1/l1_pei.a
DEPEND +=	$(M23DIR)/g23m-gsm/mm/mm.a
DEPEND +=	$(M23DIR)/g23m-gsm/sms/sms.a
DEPEND +=	$(M23DIR)/g23m-gsm/sim/sim.a
DEPEND +=	$(M23DIR)/g23m-gprs/cci/cci.a
DEPEND +=	$(M23DIR)/g23m-gprs/cci/cci_ir.a
DEPEND +=	$(M23DIR)/g23m-gprs/cl/cl.a
DEPEND +=	$(M23DIR)/g23m-gprs/sm/sm.a

#DEPEND +=	$(M23DIR)/g23m-gprs/grlc/grlc.a
#DEPEND +=	$(M23DIR)/g23m-gprs/grlc/grlc_ir.a
DEPEND +=	$(M23DIR)/g23m-gprs/llc/llc.a
DEPEND +=	$(M23DIR)/g23m-gprs/gmm/gmm.a
DEPEND +=	$(M23DIR)/g23m-gprs/grr/grr.a
DEPEND +=	$(M23DIR)/g23m-gprs/sndcp/sndcp.a
DEPEND +=	$(M23DIR)/g23m-fad/fad/fad.a

DEPEND +=	$(M23DIR)/g23m-fad/ra/ra.a
DEPEND +=	$(M23DIR)/g23m-fad/rlp/rlp.a
DEPEND +=	$(M23DIR)/g23m-fad/t30/t30.a
DEPEND +=	$(M23DIR)/g23m-fad/l2r/l2r.a
DEPEND +=	$(M23DIR)/g23m-fad/app/app.a
DEPEND +=	$(M23DIR)/g23m-fad/tcpip/tcpip.a
DEPEND +=	$(M23DIR)/g23m-fad/tcpip/socket_lib.a
DEPEND +=	$(M23DIR)/g23m-fad/ip/ip.a
DEPEND +=	$(M23DIR)/g23m-fad/udp/udp.a
DEPEND +=	$(M23DIR)/g23m-fad/ppp/ppp.a
DEPEND +=	$(M23DIR)/g23m-fad/ppp/ppp_ir.a
DEPEND +=	$(M23DIR)/g23m-aci/aci/aci.a
DEPEND +=	$(M23DIR)/g23m-aci/aci_ext/aciext.a
DEPEND +=	$(M23DIR)/g23m-aci/aci_dti_mng/aci_dti_mng.a
DEPEND +=	$(M23DIR)/g23m-aci/dti/dti.a
DEPEND +=	$(M23DIR)/g23m-aci/uart/uart_b_lib.a
#DEPEND +=	$(M23DIR)/g23m-aci/ksd/ksd.a
#DEPEND +=	$(M23DIR)/g23m-aci/bat/bat.a
DEPEND +=	$(M23DIR)/g23m-aci/gdd_dio/gdd_dio.a
DEPEND +=	$(M23DIR)/g23m-aci/l2p/l2p.a
DEPEND +=	$(M23DIR)/g23m/condat/ms/src/ati_ext/atiext.a
#DEPEND +=	$(M23DIR)/g23m/condat/com/src/comlib/comlib.a
DEPEND +=	$(M23DIR)/g23m/condat/ms/src/psi/psi.a
DEPEND +=	$(M23DIR)/g23m/condat/com/src/driver/driver.a
DEPEND +=	$(M23DIR)/g23m/condat/ms/src/upm/upm.a
DEPEND +=	$(M23DIR)/g23m/nexgen/coreip/nexgen.a 

TARGET = $(CURDIR)/g23m.a

.PHONY: $(DEPEND) subdir

all: $(TARGET)

$(TARGET): $(DEPEND)
	cp $(LIBSDIR)/g23m/g23m-original.a $@
	$(AR) rvu $@ *.o
#	$(AR) $(ARFLAGS) $@ *.o
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
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci -f Makefile_cci
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci -f Makefile_cci_ir
	$(MAKE) -C $(M23DIR)/g23m-gprs/cl 
	$(MAKE) -C $(M23DIR)/g23m-gprs/sm 
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc -f Makefile_grlc
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc -f Makefile_grlc_ir
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
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp -f Makefile_ppp
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp -f Makefile_ppp_ir
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
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci -f Makefile_cci clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/cci -f Makefile_cci_ir clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/cl clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/sm clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc -f Makefile_grlc clean
	$(MAKE) -C $(M23DIR)/g23m-gprs/grlc -f Makefile_grlc_ir clean
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
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp -f Makefile_ppp clean
	$(MAKE) -C $(M23DIR)/g23m-fad/ppp -f Makefile_ppp_ir clean
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
