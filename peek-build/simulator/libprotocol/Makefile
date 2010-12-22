DEFINE += SIMULATOR
DEFINE += LINUX

INCLUDE  = $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/emobiix/protocol
INCLUDE += $(CURDIR)/asn1c-files

SRC  = $(SRCDIR)/emobiix/protocol/EmobiixField.c
SRC += $(SRCDIR)/emobiix/protocol/AuthExtraP.c
SRC += $(SRCDIR)/emobiix/protocol/AuthRequestP.c
SRC += $(SRCDIR)/emobiix/protocol/AuthResponseP.c
SRC += $(SRCDIR)/emobiix/protocol/AuthTypeP.c
SRC += $(SRCDIR)/emobiix/protocol/AuthUserPassP.c
SRC += $(SRCDIR)/emobiix/protocol/BlockSyncListP.c
SRC += $(SRCDIR)/emobiix/protocol/DataObjectStampP.c
SRC += $(SRCDIR)/emobiix/protocol/DataObjectSyncFinishP.c
SRC += $(SRCDIR)/emobiix/protocol/DataObjectSyncP.c
SRC += $(SRCDIR)/emobiix/protocol/DataObjectSyncStartP.c
SRC += $(SRCDIR)/emobiix/protocol/DataObjectTypeP.c
SRC += $(SRCDIR)/emobiix/protocol/FRIPacketP.c
SRC += $(SRCDIR)/emobiix/protocol/FieldNameP.c
SRC += $(SRCDIR)/emobiix/protocol/ProtocolHandshakeP.c
SRC += $(SRCDIR)/emobiix/protocol/ProtocolVersionP.c
SRC += $(SRCDIR)/emobiix/protocol/RecordIDP.c
SRC += $(SRCDIR)/emobiix/protocol/RecordSyncListP.c
SRC += $(SRCDIR)/emobiix/protocol/RequestResponseP.c
SRC += $(SRCDIR)/emobiix/protocol/SubscriptionRequestP.c
SRC += $(SRCDIR)/emobiix/protocol/SubscriptionResponseP.c
SRC += $(SRCDIR)/emobiix/protocol/SyncListP.c
SRC += $(SRCDIR)/emobiix/protocol/SyncOperandP.c
SRC += $(SRCDIR)/emobiix/protocol/SyncOperationP.c
SRC += $(SRCDIR)/emobiix/protocol/URLP.c
SRC += $(CURDIR)/asn1c-files/ANY.c
SRC += $(CURDIR)/asn1c-files/BIT_STRING.c
SRC += $(CURDIR)/asn1c-files/BMPString.c
SRC += $(CURDIR)/asn1c-files/BOOLEAN.c
SRC += $(CURDIR)/asn1c-files/ENUMERATED.c
SRC += $(CURDIR)/asn1c-files/GeneralString.c
SRC += $(CURDIR)/asn1c-files/GeneralizedTime.c
SRC += $(CURDIR)/asn1c-files/GraphicString.c
SRC += $(CURDIR)/asn1c-files/IA5String.c
SRC += $(CURDIR)/asn1c-files/INTEGER.c
SRC += $(CURDIR)/asn1c-files/ISO646String.c
SRC += $(CURDIR)/asn1c-files/NULL.c
SRC += $(CURDIR)/asn1c-files/NativeEnumerated.c
SRC += $(CURDIR)/asn1c-files/NativeInteger.c
SRC += $(CURDIR)/asn1c-files/NativeReal.c
SRC += $(CURDIR)/asn1c-files/NumericString.c
SRC += $(CURDIR)/asn1c-files/OBJECT_IDENTIFIER.c
SRC += $(CURDIR)/asn1c-files/OCTET_STRING.c
SRC += $(CURDIR)/asn1c-files/ObjectDescriptor.c
SRC += $(CURDIR)/asn1c-files/PrintableString.c
SRC += $(CURDIR)/asn1c-files/REAL.c
SRC += $(CURDIR)/asn1c-files/RELATIVE-OID.c
SRC += $(CURDIR)/asn1c-files/T61String.c
SRC += $(CURDIR)/asn1c-files/TeletexString.c
SRC += $(CURDIR)/asn1c-files/UTCTime.c
SRC += $(CURDIR)/asn1c-files/UTF8String.c
SRC += $(CURDIR)/asn1c-files/UniversalString.c
SRC += $(CURDIR)/asn1c-files/VideotexString.c
SRC += $(CURDIR)/asn1c-files/VisibleString.c
SRC += $(CURDIR)/asn1c-files/asn_SEQUENCE_OF.c
SRC += $(CURDIR)/asn1c-files/asn_SET_OF.c
SRC += $(CURDIR)/asn1c-files/asn_codecs_prim.c
SRC += $(CURDIR)/asn1c-files/ber_decoder.c
SRC += $(CURDIR)/asn1c-files/ber_tlv_length.c
SRC += $(CURDIR)/asn1c-files/ber_tlv_tag.c
SRC += $(CURDIR)/asn1c-files/constr_CHOICE.c
SRC += $(CURDIR)/asn1c-files/constr_SEQUENCE.c
SRC += $(CURDIR)/asn1c-files/constr_SEQUENCE_OF.c
SRC += $(CURDIR)/asn1c-files/constr_SET.c
SRC += $(CURDIR)/asn1c-files/constr_SET_OF.c
SRC += $(CURDIR)/asn1c-files/constr_TYPE.c
SRC += $(CURDIR)/asn1c-files/constraints.c
SRC += $(CURDIR)/asn1c-files/der_encoder.c
SRC += $(CURDIR)/asn1c-files/per_decoder.c
SRC += $(CURDIR)/asn1c-files/per_encoder.c
SRC += $(CURDIR)/asn1c-files/per_opentype.c
SRC += $(CURDIR)/asn1c-files/per_support.c
SRC += $(CURDIR)/asn1c-files/xer_decoder.c
SRC += $(CURDIR)/asn1c-files/xer_encoder.c
SRC += $(CURDIR)/asn1c-files/xer_support.c

OBJ = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC)))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libprotocol.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules
