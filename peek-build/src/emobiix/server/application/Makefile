TARGET = application.a

INCLUDE  = $(BOOSTINC)
INCLUDE += $(ASN1CINC)
INCLUDE += $(GSOAPINC)
INCLUDE += ./include
INCLUDE += $(BASEDIR)/protocol
INCLUDE += $(BASEDIR)/utils/include
INCLUDE += $(BASEDIR)/soaprpc/include

DEFINE = BOOST_ALL_NO_LIB=1

OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET) $^; $(RANLIB) $@

clean:
	$(RM) $(TARGET) *.o *.d

include $(BASEDIR)/Makefile.rules
