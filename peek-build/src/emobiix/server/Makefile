include Makefile.globals

TARGET=server
SUBDIR=protocol application utils soaprpc purple

INCLUDE  = $(GSOAPINC)
INCLUDE += $(BOOSTINC)
INCLUDE += utils/include
INCLUDE += protocol
INCLUDE += application/include
INCLUDE += soaprpc

DEFINE  = WITH_OPENSSL
DEFINE += WITH_CDATA 
DEFINE += DEBUG
DEFINE += WITH_GZIP

OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

LIB  = application/application.a
LIB += protocol/protocol.a
LIB += utils/utils.a
LIB += soaprpc/soaprpc.a
LIB += soaprpc/soapserve.a
LIB += $(GSOAPLIB)/libgsoapssl++.a -lssl -lcrypto -lz
LIB += $(BOOSTLIB)/libboost_system.a
LIB += $(BOOSTLIB)/libboost_thread.a -lpthread
LIB += -lxerces-c

.PHONY: $(SUBDIR) peek purple peekclean

all: server

server: $(SUBDIR) $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIB) 

application: utils protocol soaprpc
	$(MAKE) -C $@

protocol soaprpc:
	$(MAKE) -C $@ prepare
	$(MAKE) -C $@

utils: 
	$(MAKE) -C $@

peek:
	$(MAKE) -C examples/peek_interface

peekclean:
	$(MAKE) -C examples/peek_interface clean

purple:
	$(MAKE) -C examples/purple-server

purpleclean:
	$(MAKE) -C examples/purple-server clean

clean: 
	$(MAKE) -C protocol clean
	$(MAKE) -C application clean
	$(MAKE) -C utils clean
	$(MAKE) -C soaprpc clean
	$(MAKE) -C examples/purple-server clean
	$(RM) $(TARGET) *.o *.d

include $(BASEDIR)/Makefile.rules
