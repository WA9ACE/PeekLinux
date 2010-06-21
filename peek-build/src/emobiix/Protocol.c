#include "Protocol.h"

#include "Debug.h"

#include "p_malloc.h"

void protocol_syncStart(DataObjectSyncStartP_t *p, const char *url,
		DataObjectStampP_t minor, DataObjectStampP_t major, long sequenceID)
{
	p->urlP.buf = (uint8_t *)url;
	p->urlP.size = strlen(url);
	p->dataObjectStampMinorP = minor;
	p->dataObjectStampMajorP = major;
	p->syncSequenceIDP = sequenceID;
}

void protocol_syncFinished(DataObjectSyncFinishP_t *p,
		RequestResponseP_t response, long sequenceID)
{
	p->responseP = response;
	p->syncSequenceIDP = sequenceID;
}

SyncOperandP_t *protocol_serializeField(DataObject *sobj, const char *fieldName)
{
	SyncOperandP_t *syncOp;
	DataObjectField *field;

	syncOp = (SyncOperandP_t *)p_malloc(sizeof(SyncOperandP_t));
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, fieldName);
    field = dataobject_getValue(sobj, fieldName);
	if (field->type == DOF_STRING) {
		syncOp->syncP.present = syncP_PR_syncSetP;
		syncOp->syncP.choice.syncSetP.buf = NULL;
        OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncSetP,
				field->field.string, strlen(field->field.string));
	} else if (field->type == DOF_DATA) {
		syncOp->syncP.present = syncP_PR_syncModifyP;
		syncOp->syncP.choice.syncModifyP.modifyDataP.buf = NULL;
        OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncModifyP.modifyDataP, 
                (const char *)field->field.data.bytes,
                field->field.data.size);
		syncOp->syncP.choice.syncModifyP.modifySizeP = field->field.data.size;
		syncOp->syncP.choice.syncModifyP.modifyOffsetP = 0;
	} else {
        emo_printf("Unsupported field type in sync" NL);
		p_free(syncOp);
		return NULL;
	}

	return syncOp;
}

SyncOperandP_t *protocol_goToTree(int tree)
{
	SyncOperandP_t *syncOp;

	syncOp = (SyncOperandP_t *)p_malloc(sizeof(SyncOperandP_t));

	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, "");
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeGotoTreeP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP = tree;

	return syncOp;
}

SyncOperandP_t *protocol_addChild(void)
{
	SyncOperandP_t *syncOp;

	syncOp = (SyncOperandP_t *)p_malloc(sizeof(SyncOperandP_t));

	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, "");
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeAddP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeAddP = nodeAddP_nodeChildP;

	return syncOp;
}

void protocol_blockSyncList(DataObjectSyncP_t *p)
{
	p->syncListP.present = SyncListP_PR_blockSyncListP;
	p->syncListP.choice.blockSyncListP.list.array = NULL;
	p->syncListP.choice.blockSyncListP.list.size = 0;
	p->syncListP.choice.blockSyncListP.list.count = 0;
	p->syncListP.choice.blockSyncListP.list.free = NULL;
}

void protocol_recordSyncList(DataObjectSyncP_t *p)
{
	p->syncListP.present = SyncListP_PR_recordSyncListP;
	p->syncListP.choice.recordSyncListP.list.array = NULL;
	p->syncListP.choice.recordSyncListP.list.size = 0;
	p->syncListP.choice.recordSyncListP.list.count = 0;
	p->syncListP.choice.recordSyncListP.list.free = NULL;
}

void protocol_authUserPass(AuthUserPassP_t *p,
		const char *username, const char *pass)
{
	p->authUsernameP.buf = NULL;
	OCTET_STRING_fromString(&p->authUsernameP, username);
	p->authPasswordP.buf = NULL;
	OCTET_STRING_fromString(&p->authPasswordP, pass);
	p->authTypeP = AuthTypeP_atUsernamePasswordP;

	p->authExtrasP.list.array = NULL;
	p->authExtrasP.list.size = 0;
	p->authExtrasP.list.count = 0;
	p->authExtrasP.list.free = NULL;
}

void protocol_autUserPassExtra(AuthUserPassP_t *p,
		const char *name, const char *value)
{
	AuthExtraP_t *extra;

	extra = (AuthExtraP_t *)p_malloc(sizeof(AuthExtraP_t));
	extra->authExtraNameP.buf = NULL;
	OCTET_STRING_fromString(&extra->authExtraNameP, name);
	extra->authExtraValueP.buf = NULL;
	OCTET_STRING_fromString(&extra->authExtraValueP, value);
	asn_sequence_add(&p->authExtrasP.list, extra);
}

void protocol_authResponse(AuthResponseP_t *p, long response)
{
	*p = response;
}

