DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/bmi/gdi/gdi.a \
		$(M23DIR)/bmi/Condat/MS/SRC/bmi/bmi-mmi.a \
		$(M23DIR)/bmi/Condat/MS/SRC/bmi/Icons/bmi-Icons.a \
		$(M23DIR)/bmi/Condat/MS/SRC/atb/bmi-atb.a \
		$(M23DIR)/bmi/Condat/MS/SRC/mfw/bmi-mfw.a

TARGET = $(CURDIR)/bmi.a

.PHONY: $(DEPEND) subdir

all: $(TARGET)

$(TARGET): $(DEPEND)
	$(AR) $(ARFLAGS) $@ *.o
	$(RM) *.o

$(DEPEND): subdir
	$(AR) -xv $@

subdir:
	$(MAKE) -C $(M23DIR)/bmi/gdi
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi/Icons
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/atb
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/mfw

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C $(M23DIR)/bmi/gdi clean
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi clean
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi/Icons clean
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/atb clean
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/mfw clean

include $(BASEDIR)/Makefile.rules
