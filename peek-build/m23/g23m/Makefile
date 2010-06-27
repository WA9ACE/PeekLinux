DEFINE += 
UNDEFINE +=

DEPEND = ../g23m-aci/aci/aci.a
SUBDIR = ../g23m-aci/aci

TARGET = $(SRCDIR)/g23m.a

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
