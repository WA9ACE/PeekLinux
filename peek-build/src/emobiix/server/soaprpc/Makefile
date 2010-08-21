TARGET = soaprpc.a

SERVERLIB = soapserve.a

SERVER = test_server
CLIENT = test_client

INCLUDE  = ./ 
INCLUDE += ./include
INCLUDE += $(SRCDIR)/utils/include
INCLUDE += $(GSOAPINC)

DEFINE  = WITH_GZIP
DEFINE += WITH_NOCDATA
DEFINE += WITH_OPENSSL

OBJ = emobiix.o soap_request.o emobiix_rpc_Client.o emobiix_rpc_C.o
SOBJ = emobiix_rpc_Server.o

SERVER_OBJ = emobiix.o test_server.o emobiix_rpc_C.o
CLIENT_OBJ = test_client.o

LIB  = $(GSOAPLIB)/libgsoapssl++.a -lssl -lcrypto -lz
LIB += /usr/lib/libcurl.a -lidn
LIB += $(SRCDIR)/utils/utils.a -lxerces-c

all: $(TARGET) $(SERVERLIB) $(SERVER) $(CLIENT)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET) $^; $(RANLIB) $@

$(SERVERLIB): $(SOBJ)
	$(AR) $(ARFLAGS) $(SERVERLIB) $^; $(RANLIB) $@

$(SERVER): $(SERVER_OBJ) $(SERVERLIB)
	$(CXX) -o $(SERVER) $(SERVER_OBJ) $(SERVERLIB) $(LIB)

$(CLIENT): $(CLIENT_OBJ) $(TARGET)
	$(CXX) -o $(CLIENT) $(CLIENT_OBJ) $(TARGET) $(LIB)

prepare:
	$(GSOAPGEN) -I$(GSOAPDIR)/share/gsoap/import -L -p emobiix_rpc_ include/emobiix.h

clean:
	$(RM) $(TARGET) $(SERVERLIB) $(SERVER) $(CLIENT) *.o *.d emobiix_rpc_* emobiix.nsmap emobiix*.xml *.wsdl *.xsd

include $(BASEDIR)/Makefile.rules
