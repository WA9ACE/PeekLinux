#include "ProtocolUtils.h"

#include <asn_application.h>
#include "SyncOperandP.h"

#include "p_malloc.h"

void protocolFreeFRIPacketP(FRIPacketP_t *p)
{
	protocolFreeFRIPacketP_children(p);
	p_free(p);
}

void protocolFreeFRIPacketP_children(FRIPacketP_t *p)
{
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
	protocolFreeDataObjectSyncP_children(p);
	p_free(p);
}

void protocolFreeDataObjectSyncP_children(DataObjectSyncP_t *p)
{
	if (p->syncListP.present == SyncListP_PR_blockSyncListP) {
		asn_sequence_empty(&p->syncListP.choice.blockSyncListP.list);
	} else {
		asn_sequence_empty(&p->syncListP.choice.recordSyncListP.list);
	}
}

char *OCTET_STRING_to_string(OCTET_STRING_t *o)
{
	char *tmpstr;

	tmpstr = p_malloc(o->size+1);
	if (tmpstr == NULL)
		return NULL;
	tmpstr[o->size] = 0;
	memcpy(tmpstr, o->buf, o->size);

	return tmpstr;
}

