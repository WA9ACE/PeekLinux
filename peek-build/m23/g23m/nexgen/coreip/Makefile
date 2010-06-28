DEFINE += RNET_CFG_REAL_TRANSPORT

UNDEFINE += L1
UNDEFINE += RELEASE99

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m/nexgen/coreip
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngip
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngos
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngos/cpu
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngos/cpu/arm
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngos/cpu/i386
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngresolv
INCLUDE += $(M23DIR)/g23m/nexgen/coreip/ngos/cpu
INCLUDE += $(M23DIR)/g23m-fad/tcpip
INCLUDE += $(M23DIR)/g23m-fad/tcpip/include
INCLUDE += $(M23DIR)/g23m-fad/tcpip/rnet
INCLUDE += $(M23DIR)/g23m-fad/tcpip/rnet/rnet_rt
INCLUDE += $(M23DIR)/g23m-fad/tcpip/rnet/rnet_br
INCLUDE += $(M23DIR)/g23m-fad/tcpip/rnet/rnet_ws


SRC  = rnet_rt_ng_api_rbaa.c
SRC += rnet_rt_ng_api_rbna.c
SRC += rnet_rt_ng_arp.c
SRC += rnet_rt_ng_arp_look.c
SRC += rnet_rt_ng_arp_req.c
SRC += rnet_rt_ng_atoh.c
SRC += rnet_rt_ng_atoi.c
SRC += rnet_rt_ng_atol.c
SRC += rnet_rt_ng_aton.c
SRC += rnet_rt_ng_buf_copy.c
SRC += rnet_rt_ng_cksum.c
SRC += rnet_rt_ng_debug.c
SRC += rnet_rt_ng_error.c
SRC += rnet_rt_ng_eth_cntl.c
SRC += rnet_rt_ng_eth_crc.c
SRC += rnet_rt_ng_eth_init.c
SRC += rnet_rt_ng_eth_inp.c
SRC += rnet_rt_ng_eth_out.c
SRC += rnet_rt_ng_getver.c
SRC += rnet_rt_ng_if_cntl.c
SRC += rnet_rt_ng_ifinput.c
SRC += rnet_rt_ng_igmp_jon.c
SRC += rnet_rt_ng_igmp_lev.c
SRC += rnet_rt_ng_igmp_snd.c
SRC += rnet_rt_ng_in_cksum.c
SRC += rnet_rt_ng_init.c
SRC += rnet_rt_ng_input.c
SRC += rnet_rt_ng_iovcpy.c
SRC += rnet_rt_ng_ip.c
SRC += rnet_rt_ng_ip_data.c
SRC += rnet_rt_ng_ip_loop.c
SRC += rnet_rt_ng_ip_out.c
SRC += rnet_rt_ng_ip_rtupd.c
SRC += rnet_rt_ng_ip_ver.c
SRC += rnet_rt_ng_loopback.c
SRC += rnet_rt_ng_mbk_allo.c
SRC += rnet_rt_ng_mbk_free.c
SRC += rnet_rt_ng_mbk_init.c
SRC += rnet_rt_ng_ntoa.c
SRC += rnet_rt_ng_random.c
SRC += rnet_rt_ng_rawip.c
SRC += rnet_rt_ng_rawip_dt.c
SRC += rnet_rt_ng_rawprint.c
SRC += rnet_rt_ng_res_auto.c
SRC += rnet_rt_ng_res_cach.c
SRC += rnet_rt_ng_res_prot.c
SRC += rnet_rt_ng_res_quer.c
SRC += rnet_rt_ng_res_star.c
SRC += rnet_rt_ng_res_util.c
SRC += rnet_rt_ng_sio_acce.c
SRC += rnet_rt_ng_sio_balo.c
SRC += rnet_rt_ng_sio_bfre.c
SRC += rnet_rt_ng_sio_bind.c
SRC += rnet_rt_ng_sio_brcv.c
SRC += rnet_rt_ng_sio_bsnd.c
SRC += rnet_rt_ng_sio_clos.c
SRC += rnet_rt_ng_sio_conn.c
SRC += rnet_rt_ng_sio_crea.c
SRC += rnet_rt_ng_sio_lstn.c
SRC += rnet_rt_ng_sio_opt.c
SRC += rnet_rt_ng_sio_rcvv.c
SRC += rnet_rt_ng_sio_recv.c
SRC += rnet_rt_ng_sio_scb.c
SRC += rnet_rt_ng_sio_send.c
SRC += rnet_rt_ng_sio_shtd.c
SRC += rnet_rt_ng_sio_sndv.c
SRC += rnet_rt_ng_sio_sopt.c
SRC += rnet_rt_ng_so_alloc.c
SRC += rnet_rt_ng_so_bind.c
SRC += rnet_rt_ng_so_conn.c
SRC += rnet_rt_ng_so_getev.c
SRC += rnet_rt_ng_so_iscnc.c
SRC += rnet_rt_ng_so_look.c
SRC += rnet_rt_ng_so_sleep.c
SRC += rnet_rt_ng_sprintf.c
SRC += rnet_rt_ng_start.c
SRC += rnet_rt_ng_stop.c
SRC += rnet_rt_ng_strerror.c
SRC += rnet_rt_ng_strtok.c
SRC += rnet_rt_ng_tcp.c
SRC += rnet_rt_ng_tcp_data.c
SRC += rnet_rt_ng_tcp_out.c
SRC += rnet_rt_ng_tcp_subr.c
SRC += rnet_rt_ng_timer.c
SRC += rnet_rt_ng_udp.c
SRC += rnet_rt_ng_udp_data.c
SRC += rnet_rt_ng_vsprintf.c
SRC += rnet_rt_ngos.c
#SRC += rnet_rt_ngwinnet.c
SRC += rnet_rt_ng_ip_rtlku.c
SRC += rnet_rt_ng_if_buf.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/nexgen.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
