DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = dma_api.c
SRC += dma_i.c
SRC += dma_handle_message.c
SRC += dma_handle_timer.c
SRC += dma_pei.c
SRC += board/dma_inth.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/dma.a

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
