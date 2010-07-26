DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = dsp_dwnld/leadapi.c
SRC += inth/niq.c
SRC += inth/inth.c
SRC += i2c/bspI2cDelay.c
SRC += i2c/bspI2cMultiMaster.c
SRC += i2c/i2c_pwr.c
SRC += spi/spi_drv.c
SRC += abb/bspTwl3029.c
SRC += abb/bspTwl3029_Audio.c
SRC += abb/bspTwl3029_Aux.c
SRC += abb/bspTwl3029_I2c.c
SRC += abb/bspTwl3029_Power.c
SRC += abb/bspTwl3029_Rtc.c
SRC += abb/bspTwl3029_Sim.c
SRC += abb/bspTwl3029_UsbOtg.c
SRC += abb/bspTwl3029_IntC.c
SRC += abb/bspTwl3029_Bci.c
SRC += abb/bspTwl3029_Madc.c
SRC += abb/pg2x/bspTwl3029_Audio_pg2x.c
SRC += abb/pg2x/bspTwl3029_Aux_pg2x.c
SRC += abb/pg2x/bspTwl3029_I2c_pg2x.c
SRC += abb/pg2x/bspTwl3029_IntC_pg2x.c
SRC += abb/pg2x/bspTwl3029_UsbOtg_pg2x.c
SRC += inth/sys_inth32.c
SRC += security/certificate.c
SRC += inth/niq32.c
SRC += uart/uart.c
SRC += timer/timer.c
SRC += timer/timer1.c
SRC += timer/timer2.c
SRC += timer/timer_sec.c
SRC += clkm/clkm.c
SRC += armio/armio.c
SRC += dma/sys_dma.c
SRC += dma/sys_dma_it.c
SRC += inth/sys_inth.c
SRC += memif/sys_memif.c

CCTHUMB=

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/drivers_flash.a

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
