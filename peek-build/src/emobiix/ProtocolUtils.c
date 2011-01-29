#include "ProtocolUtils.h"

#include <asn_application.h>
#include "SyncOperandP.h"
#include "Debug.h"

#include "p_malloc.h"

void protocolFreeFRIPacketP(FRIPacketP_t *p)
{
	EMO_ASSERT(p != NULL, "protocol free packet missing packet")
	protocolFreeFRIPacketP_children(p);
	p_free(p);
}

void protocolFreeFRIPacketP_children(FRIPacketP_t *p)
{
	EMO_ASSERT(p != NULL, "protocol free packet children missing packet")

	switch (p->packetTypeP.present) {
		case packetTypeP_PR_protocolHandshakeP:
			break;
		case packetTypeP_PR_authRequestP:
			break;
		case packetTypeP_PR_authUserPassP:
			break;
		case packetTypeP_PR_authResponseP:
			break;
		case packetTypeP_PR_subscriptionRequestP:
			break;
		case packetTypeP_PR_subscriptionResponseP:
			break;
		case packetTypeP_PR_dataObjectSyncStartP:

			break;
		case packetTypeP_PR_dataObjectSyncP:
			protocolFreeDataObjectSyncP_children(&p->packetTypeP.choice.dataObjectSyncP);
			break;
		case packetTypeP_PR_dataObjectSyncFinishP:

			break;
		default:
			break;
	}
}

void protocolFreeDataObjectSyncP(DataObjectSyncP_t *p)
{
	EMO_ASSERT(p != NULL, "protocol free DataObjectSync missing packet")

	protocolFreeDataObjectSyncP_children(p);
	p_free(p);
}

void protocolFreeDataObjectSyncP_children(DataObjectSyncP_t *p)
{
	EMO_ASSERT(p != NULL,"protocol free DataObjectSync children missing packet")

	if (p->syncListP.present == SyncListP_PR_blockSyncListP) {
		asn_sequence_empty(&p->syncListP.choice.blockSyncListP.list);
	} else {
		asn_sequence_empty(&p->syncListP.choice.recordSyncListP.list);
	}
}

char *OCTET_STRING_to_string(OCTET_STRING_t *o)
{
	char *tmpstr;

	EMO_ASSERT_NULL(o != NULL, "OCTET_STRING to string missing OCTET_STRING")

	tmpstr = p_malloc(o->size+1);
	if (tmpstr == NULL)
		return NULL;
	tmpstr[o->size] = 0;
	memcpy(tmpstr, o->buf, o->size);

	return tmpstr;
}

static const char *itoa_alphabet =
        "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";

void emo_itoa(int value, char* result, int base)
{
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    if (base < 2 || base > 36) { 
        *result = '\0';
        return;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = itoa_alphabet[35 + (tmp_value - value * base)];
    } while (value);

    if (tmp_value < 0)
        *ptr++ = '-';

    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}

void emo_uitoa(unsigned int value, char* result, int base)
{
    char* ptr = result, *ptr1 = result, tmp_char;
    unsigned int tmp_value;

    if (base < 2 || base > 36) { 
        *result = '\0';
        return;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = itoa_alphabet[35 + (tmp_value - value * base)];
    } while (value);

    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}