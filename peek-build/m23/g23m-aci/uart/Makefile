DEFINE += 
UNDEFINE += 

INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/csw-system/drv_core/uart
INCLUDE += $(M23DIR)/g23m-aci/uart

SRC  = uart_kerf.c  
SRC += uart_kerp.c  
SRC += uart_kers.c  
SRC += uart_drxf.c  
SRC += uart_drxp.c  
SRC += uart_drxs.c  
SRC += uart_dtxf.c  
SRC += uart_dtxp.c  
SRC += uart_dtxs.c  
SRC += uart_rxf.c  
SRC += uart_rxp.c  
SRC += uart_rxs.c  
SRC += uart_txf.c  
SRC += uart_txp.c  
SRC += uart_txs.c  
SRC += uart_rtf.c  
SRC += uart_rtp.c  
SRC += uart_rts.c  
SRC += uart_pei.c 

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/uart_b_lib.a

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
