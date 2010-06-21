#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "DataObject.h"
#include "Application.h"

#include "FRIPacketP.h"
#include "SyncOperandP.h"
#include "ProtocolUtils.h"

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

void protocol_syncStart(DataObjectSyncStartP_t *p, const char *url,
		DataObjectStampP_t minor, DataObjectStampP_t major, long sequenceID);
void protocol_syncFinished(DataObjectSyncFinishP_t *p,
		RequestResponseP_t response, long sequenceID);
SyncOperandP_t *protocol_serializeField(DataObject *dobj, const char *fieldName);
SyncOperandP_t *protocol_goToTree(int tree);
SyncOperandP_t *protocol_addChild(void);
void protocol_blockSyncList(DataObjectSyncP_t *p);
void protocol_recordSyncList(DataObjectSyncP_t *p);
RecordSyncListP_t *protocol_recordSync(int isDelete, unsigned int stampMinor,
		unsigned int stampMajor);
void protocol_authUserPass(AuthUserPassP_t *p,
		const char *username, const char *pass);
void protocol_autUserPassExtra(AuthUserPassP_t *p,
		const char *name, const char *value);
void protocol_authResponse(AuthResponseP_t *p, long response);

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_H_ */
