DEFINE += 
UNDEFINE +=

DEPEND =	$(M23DIR)/bmi/gdi/gdi.a \
		$(M23DIR)/bmi/Condat/MS/SRC/bmi/bmi-mmi.a

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

clean: 
	$(RM) $(TARGET)
	$(RM) *.o
	$(MAKE) -C $(M23DIR)/bmi/gdi clean
	$(MAKE) -C $(M23DIR)/bmi/Condat/MS/SRC/bmi clean

include $(BASEDIR)/Makefile.rules
