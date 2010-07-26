DEFINE += NTRACE 
DEFINE += _TARGET 
DEFINE += NEW_ENTITY 
DEFINE += SHARED_VSI 
DEFINE += SMI 
DEFINE += NWARN 
DEFINE += OPTION_MULTITHREAD 
DEFINE += NEW_FRAME

UNDEFINE += FF_EM_MOD
UNDEFINE += FF_EM_MODE

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/L1/layer1/p_include
INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/g23m/condat/com/include

SRC  = cst_csf.c
SRC += cst_exts.c
SRC += cst_pei.c
SRC += cst_stack.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/cst.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules
