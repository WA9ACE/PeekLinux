#include "ConnectionContext.h"

#include "Map.h"
#include "DataObject.h"
#include "List.h"
#include "Widget.h"
#include "Debug.h"
#include "Mime.h"
#include "Protocol.h"
#include "ProtocolUtils.h"
#include "Application.h"
#include "ApplicationManager.h"

#include "Style.h"

#include "p_malloc.h"

#ifndef SIMULATOR
#include "balimeiapi.h"
#endif

static const int CCTX_BUFLEN = 4096;

struct SyncRequest_t {
	URL *url;
	int isClient;
	int hasStarted;
	int hasFinished;
	int remoteFinished;
	int finalize;
	RequestResponseP_t remoteResponse;
	unsigned int stampMinor;
	unsigned int stampMajor;
	
	DataObject *dobj;
	List *completeFields;
	List *completeNodes;
	unsigned int sequenceID;

	int objectIndex;
	int recordIndex;
	int childOp;
};
typedef struct SyncRequest_t SyncRequest;

static SyncRequest *syncRequest_new(URL *url, int isClient);
static void syncRqeuest_delete(SyncRequest *sreq);

struct ConnectionContext_t {
	Endpoint *endpoint;
	Map *syncRequests;
	List *inprogressRequests;
	unsigned char *buffer;
	int bufferBytes;
	asn_codec_ctx_t decodeContext;
	enum {NA_UNKNOWN, NA_NO, NA_YES} needAuth;
	int hasAuth;
};

static void connectionContext_processPacket(ConnectionContext *ctx,
		FRIPacketP_t *packet);
static void connectionContext_syncPacket(ConnectionContext *ctx,
		DataObjectSyncP_t *packet);
static void connectionContext_processSyncRequest(ConnectionContext *ctx,
		SyncRequest *sreq);
static void connectionContext_packetSend(ConnectionContext *ctx,
		FRIPacketP_t *packet);
static void connectionContext_processSyncStart(ConnectionContext *ctx,
		DataObjectSyncStartP_t *p);
static void connectionContext_outgoingSync(ConnectionContext *ctx,
		SyncRequest *sreq, FRIPacketP_t *p);
static void connectionContext_processSync(ConnectionContext *ctx,
		DataObjectSyncP_t *p);
static void connectionContext_processSyncOperand(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *sobj, SyncOperandP_t *syncOp);
static void connectionContext_processBlockSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *sobj, struct blockSyncListP *p);
static void connectionContext_processRecordSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, struct recordSyncListP *p);
static void connectionContext_processAuthRequest(ConnectionContext *ctx,
		AuthRequestP_t *p);
static void connectionContext_authUserPass(ConnectionContext *ctx);
static void connectionContext_processAuthUserPass(ConnectionContext *ctx,
		AuthUserPassP_t *p);
static void connectionContext_processAuthResponse(ConnectionContext *ctx,
		AuthResponseP_t *p);
static void connectionContext_recordSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, SyncListP_t *p);
static RecordSyncListP_t *connectionContext_recordSync(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *dobj);
static void connectionContext_syncOperand(ConnectionContext *ctx,
		SyncRequest *sreq, SyncListP_t *list, RecordSyncListP_t *listr,
		DataObject *sobj);

static const char *generate_mapKey(Endpoint *ep, long sid)
{
	static char mapKey[64];
	snprintf(mapKey, 64, "%p,%d", ep, sid);
	return mapKey;
}

/*static int encode_out(const void *buffer, size_t size, ConnectionContext *ctx)
{
	Transport *transport;

	emo_printf( "Sending %d bytes" NL, size);
	transport = endpoint_getTransport(ctx->endpoint);
	return (transport->write(ctx->endpoint, buffer, size) == size) ? 0 : -1;
}*/

ConnectionContext *connectionContext_new(Endpoint *ep)
{
	Map *map;
	ConnectionContext *output;
	unsigned char *buffer;
	List *list;

	map = map_string();
	if (map == NULL)
		return NULL;

	buffer = p_malloc(CCTX_BUFLEN);
	if (buffer == NULL) {
		map_delete(map);
		return NULL;
	}

	list = list_new();
	if (list == NULL) {
		map_delete(map);
		p_free(buffer);
		return NULL;
	}

	output = p_malloc(sizeof(ConnectionContext));
	if (output == NULL) {
		map_delete(map);
		p_free(buffer);
		list_delete(list);
		return NULL;
	}
	output->endpoint = ep;
	output->syncRequests = map;
	output->buffer = buffer;
	output->bufferBytes = 0;
	output->decodeContext.max_stack_size = 30000;
	output->inprogressRequests = list;
	output->needAuth = NA_UNKNOWN;
	output->hasAuth = 0;

	return output;
}

void connectionContext_delete(ConnectionContext *ctx)
{
	map_delete(ctx->syncRequests);
	p_free(ctx->buffer);
	p_free(ctx);
}

/*
 * we return after trying only one action in here.
 * this is so we dont try and do too much since the
 * UI could get bogged down if we hog CPU
 */
int connectionContext_loopIteration(ConnectionContext *ctx)
{
	Transport *transport;
	int response;
	MapIterator *iter;
	void *key;
	SyncRequest *sreq;

	transport = endpoint_getTransport(ctx->endpoint);

	/*emo_printf("Loop iteration...");*/

	/* first check if there is any inbound data pending on our connection */
	response = transport->peek(ctx->endpoint, ctx->buffer+ctx->bufferBytes, 1);
	if (response < 0) {
        /*emo_printf("transport->peek respose: %d\n", response);*/
		/* error state, or no data */
	} else if (response > 0) {
		response = transport->read(ctx->endpoint, ctx->buffer+ctx->bufferBytes, CCTX_BUFLEN-ctx->bufferBytes);
		emo_printf("ANDREY::Read %d bytes" NL, response);
		if (response < 0) {
			emo_printf("ANDREY::read got error state" NL);
			/* error state */
		} else if (response > 0) {
			ctx->bufferBytes += response;
			do {
				response = connectionContext_consumePacket(ctx);
				emo_printf("Consumed %d bytes" NL, response);
			} while (response > 0);
			return 1;
		}
	}

	if (!((ctx->needAuth == NA_YES && ctx->hasAuth) || ctx->needAuth == NA_NO)) {
		emo_printf("Not doing outgoing sync because we are not authorized" NL);
		return 0;
	}


	/*emo_printf("Outgoing sync");*/

	/* perform any outgoing sync requests */
	iter = map_begin(ctx->syncRequests);
	while (!mapIterator_finished(iter)) {
		sreq = (SyncRequest *)mapIterator_item(iter, &key);
#if 0
		emo_printf("Processing sync request %s" NL, sreq->url->all);
#endif
		connectionContext_processSyncRequest(ctx, sreq);
		if (sreq->finalize)
			mapIterator_remove(iter);
		else
			mapIterator_next(iter);
		return 1;
	}

	/*emo_printf("Outgoing sync complete");*/

	return 0;
}

void connectionContext_requestAuth(ConnectionContext *ctx)
{
	FRIPacketP_t packet;
	AuthRequestP_t *p;
	AuthTypeP_t authType;

	ctx->needAuth = NA_YES;
	ctx->hasAuth = 0;
	authType = AuthTypeP_atUsernamePasswordP;

	packet.packetTypeP.present = packetTypeP_PR_authRequestP;
	p = &packet.packetTypeP.choice.authRequestP;

	p->authSaltP.buf = NULL;
	OCTET_STRING_fromString(&p->authSaltP, "Hi!!");

	p->authTypesP.list.array = NULL;
	p->authTypesP.list.size = 0;
	p->authTypesP.list.count = 0;
	p->authTypesP.list.free = NULL;

	asn_sequence_add(&p->authTypesP.list, &authType);

	emo_printf("Sending request auth" NL);
	connectionContext_packetSend(ctx, &packet);
}

int connectionContext_syncRequest(ConnectionContext *ctx, URL *url)
{
	SyncRequest *sreq;
	char mapKey[64];

	if (ctx == NULL) {
		emo_printf("ConnectionContext is NULL.." NL);
		return 0;
	}

	sreq = syncRequest_new(url, 1);
	sreq->hasStarted = 0;
	sreq->sequenceID = endpoint_getTransport(ctx->endpoint)->sequenceID(ctx->endpoint);
	snprintf(mapKey, 64, "%p,%d", ctx->endpoint, sreq->sequenceID);
	/*emo_printf("mapKey: %s" NL, mapKey);*/
	map_append(ctx->syncRequests, mapKey, sreq);

	return 1;
}

int connectionContext_consumePacket(ConnectionContext *ctx)
{
	asn_dec_rval_t retval;
	FRIPacketP_t *packet = NULL;
	int consumed;

	if (ctx->bufferBytes <= 0)
		return 0;

#if 0
	for (consumed = 0; consumed <= ctx->bufferBytes; ++consumed) {
		emo_printf( "Trying %d bytes decode\n", consumed);
		retval = uper_decode_complete(&ctx->decodeContext, &asn_DEF_FRIPacketP,
				(void **)&packet, ctx->buffer, consumed);
		if (retval.code != RC_WMORE)
			break;
	}
#else
	retval = uper_decode_complete(&ctx->decodeContext, &asn_DEF_FRIPacketP,
			(void **)&packet, ctx->buffer, ctx->bufferBytes);
#endif
	switch (retval.code) {
		case RC_OK:
			consumed = retval.consumed;
			connectionContext_processPacket(ctx, packet);
			memmove(ctx->buffer, ctx->buffer + consumed,
					ctx->bufferBytes - consumed);
			ctx->bufferBytes -= consumed;

			protocolFreeFRIPacketP(packet);
			return consumed;
		case RC_WMORE:
			emo_printf("Needs more data for a complete packet" NL);
			return 0;
		case RC_FAIL:
			emo_printf("Remote side sent unrecognized packet" NL);
			return 0;
	}
	return 0;
}

static void connectionContext_processSyncStart(ConnectionContext *ctx,
		DataObjectSyncStartP_t *p)
{
	char *tmpstr;
	DataObject *dobj;
	SyncRequest *sreq;
	URL *url;
	int isLocal;
	const char *mapKey;

	emo_printf("SyncStart packet" NL);
	tmpstr = OCTET_STRING_to_string(&p->urlP);
	if (tmpstr == NULL) {
		emo_printf("Faild to get URL in sync start" NL);
		return;
	}
	sreq = (SyncRequest *)list_find(ctx->inprogressRequests, tmpstr, (ListComparitor)strcmp);
	emo_printf("SyncStart : %s" NL, tmpstr);
	if (sreq != NULL) {
		emo_printf("Received a Sync request for a already in progress object :%s" NL,
				tmpstr);
		return;
	}
	url = url_parse(tmpstr, URL_ALL);
	if (url == NULL) {
		emo_printf("Faild to parse URL in sync start :%s" NL,
				tmpstr);
		p_free(tmpstr);
		return;
	}
	dobj = dataobject_locate(url);
	if (dobj == NULL) {
		emo_printf("Received a Sync request for non existing object :%s" NL,
				tmpstr);
		url_delete(url);
		p_free(tmpstr);
		return;
	}
	dataobject_setStamp(dobj, 0, 0);
	isLocal = dataobject_isLocal(dobj);
	sreq = syncRequest_new(url, !isLocal);
	if (sreq != NULL) {
		sreq->sequenceID = p->syncSequenceIDP;
		sreq->dobj = dobj;
		sreq->hasStarted = 1;
		sreq->stampMinor = p->dataObjectStampMinorP;
		sreq->stampMajor = p->dataObjectStampMinorP;
		mapKey = generate_mapKey(ctx->endpoint, sreq->sequenceID);
		map_append(ctx->syncRequests, mapKey, sreq);
		list_append(ctx->inprogressRequests, tmpstr);
	} else {
		emo_printf("Failed to create sync request for :%s" NL,
				tmpstr);
		url_delete(url);
		p_free(tmpstr);
	}
	p_free(tmpstr);
}

static void connectionContext_processPacket(ConnectionContext *ctx,
		FRIPacketP_t *packet)
{
	SyncRequest *sreq;
	const char *mapKey;
	DataObjectField *field;
	Application *app;

	emo_printf("Got packet : %d" NL, packet->packetTypeP.present);

	switch (packet->packetTypeP.present) {
		case packetTypeP_PR_protocolHandshakeP:
			break;
		case packetTypeP_PR_authRequestP:
			connectionContext_processAuthRequest(ctx, &packet->packetTypeP.choice.authRequestP);
			break;
		case packetTypeP_PR_authUserPassP:
			connectionContext_processAuthUserPass(ctx, &packet->packetTypeP.choice.authUserPassP);
			break;
		case packetTypeP_PR_authResponseP:
			connectionContext_processAuthResponse(ctx, &packet->packetTypeP.choice.authResponseP);
			break;
		case packetTypeP_PR_subscriptionRequestP:
			break;
		case packetTypeP_PR_subscriptionResponseP:
			break;
		case packetTypeP_PR_dataObjectSyncStartP:
			connectionContext_processSyncStart(ctx,
					&packet->packetTypeP.choice.dataObjectSyncStartP);
			break;
		case packetTypeP_PR_dataObjectSyncP:
			connectionContext_processSync(ctx, &packet->packetTypeP.choice.dataObjectSyncP);
			break;
		case packetTypeP_PR_dataObjectSyncFinishP:
			emo_printf( "DataObjectSyncFinish packet" NL);
			mapKey = generate_mapKey(ctx->endpoint, packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
			emo_printf("Map Key: %s" NL, mapKey);
			sreq = map_find(ctx->syncRequests, mapKey);
			if (sreq == NULL) {
				emo_printf("recvd sync finished for request not in progress: %d" NL, 
						packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
				break;
			}
			sreq->remoteResponse = packet->packetTypeP.choice.dataObjectSyncFinishP.responseP;
			sreq->remoteFinished = 1;
			if (sreq->isClient) {
				sreq->finalize = 1;
				dataobject_setState(sreq->dobj, DOS_OK);
				/*widget_resolveLayout(sreq->dobj, currentStyle);*/
				widget_markDirty(sreq->dobj);
				mime_loadAll(sreq->dobj);
				/*dataobject_debugPrint(sreq->dobj);*/
				field = dataobject_getValue(sreq->dobj, "type");
				if (field != NULL && field->type == DOF_STRING)
					emo_printf("Finished Type: %s" NL, field->field.string); 
				if (field != NULL && field->type == DOF_STRING &&
						strcmp(field->field.string, "application") == 0) {
					app = application_load(sreq->dobj);
					manager_launchApplication(app);
					manager_focusApplication(app);
					dataobject_debugPrint(sreq->dobj);
				}
			}
			break;
		case packetTypeP_PR_NOTHING:
		default:
			emo_printf("Unknown or NONE packet recieved" NL);
	}
}

static void connectionContext_processSyncRequest(ConnectionContext *ctx,
		SyncRequest *sreq)
{
	FRIPacketP_t packet;
	Transport *transport;

	transport = endpoint_getTransport(ctx->endpoint);

	if (sreq->isClient) {
		if (!sreq->hasStarted) {
			/* send start packet */
			/* we dont have this object at all so use stamp 0 */
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncStartP;
			protocol_syncStart(&packet.packetTypeP.choice.dataObjectSyncStartP,
					sreq->url->all, 0, 0, sreq->sequenceID);
			list_append(ctx->inprogressRequests, sreq->url->all);
			emo_printf( "Sending SyncStart" NL);
			connectionContext_packetSend(ctx, &packet);
			dataobject_setState(sreq->dobj, DOS_SYNC);
			sreq->hasStarted = 1;
			return;
		}
		if (!sreq->hasFinished) {
			/* for now, send a 'finished' packet since we dont send our changes */
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
			protocol_syncFinished(&packet.packetTypeP.choice.dataObjectSyncFinishP,
					RequestResponseP_responseExpiredP, sreq->sequenceID);
			emo_printf( "Sending SyncFinish" NL);
			connectionContext_packetSend(ctx, &packet);
			sreq->hasFinished = 1;
			return;
		}
	} else {
		if (sreq->hasStarted && sreq->remoteFinished && !sreq->hasFinished
				&& ((ctx->needAuth == NA_YES && ctx->hasAuth) || ctx->needAuth == NA_NO)){
			/* client synched to us, sync back to them */
			connectionContext_outgoingSync(ctx, sreq, &packet);
			return;
		}
		if (sreq->hasStarted && sreq->remoteFinished && sreq->hasFinished) {
			/* for now, send a 'finished' and remove request */
			emo_printf( "Sending Finish Packet" NL);
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
			protocol_syncFinished(&packet.packetTypeP.choice.dataObjectSyncFinishP,
					RequestResponseP_responseOKP, sreq->sequenceID);
			connectionContext_packetSend(ctx, &packet);
			sreq->finalize = 1;
			return;
		}
	}
}

static void connectionContext_packetSend(ConnectionContext *ctx,
		FRIPacketP_t *packet)
{
	unsigned char *buffer;
	Transport *transport;
	int size, retval;

	asn_enc_rval_t erv;
	emo_printf( "Sending packet : %d" NL, packet->packetTypeP.present);
	buffer = (unsigned char *)p_malloc(4096);
	if (buffer == NULL) {
		emo_printf("Send buffer was NULL" NL);
		return;
	}
#if 0
	erv = uper_encode(&asn_DEF_FRIPacketP, (void *)packet,
			(asn_app_consume_bytes_f *)encode_out, (void *)ctx);
#else
	erv = uper_encode_to_buffer(&asn_DEF_FRIPacketP, (void *)packet,
			buffer, 4096);
#endif
	if (erv.encoded < 0) {
		emo_printf( "Failed to encode" NL);
		p_free(buffer);
		return;
	} else
		emo_printf( "encoded: %d" NL, erv.encoded);

	size = (erv.encoded+7)/8;
	transport = endpoint_getTransport(ctx->endpoint);
	retval = transport->write(ctx->endpoint, buffer, size);
	if (retval != size)
		emo_printf("Sent %d bytes, wanted to send %d" NL, retval, size);
	p_free(buffer);
	protocolFreeFRIPacketP_children(packet);
}

static SyncRequest *syncRequest_new(URL *url, int isClient)
{
	SyncRequest *output;

	output = p_malloc(sizeof(SyncRequest));
	output->url = url;
	output->isClient = isClient;
	output->hasFinished = 0;
	output->remoteFinished = 0;
	output->finalize = 0;
	output->objectIndex = 0;
	output->recordIndex = 0;
	output->completeFields = NULL;
	output->completeNodes = list_new();
	output->dobj = dataobject_locate(url);
	output->childOp = -2;
	if (output->dobj == NULL)
		output->dobj = dataobject_construct(url, !isClient);

	return output;
}

static void syncRqeuest_delete(SyncRequest *sreq)
{
	p_free(sreq);
}

static void connectionContext_outgoingSync(ConnectionContext *ctx,
		SyncRequest *sreq, FRIPacketP_t *p)
{
	DataObject *sobj;
	
	emo_printf( "Sending Sync Packet" NL);
    p->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	if (sreq->completeFields == NULL)
		sreq->completeFields = list_new();

	p->packetTypeP.choice.dataObjectSyncP.syncSequenceIDP = sreq->sequenceID;

	if (dataobject_getRecordType(sreq->dobj)) {
		protocol_recordSyncList(&p->packetTypeP.choice.dataObjectSyncP);
		connectionContext_recordSyncList(ctx, sreq,
				&p->packetTypeP.choice.dataObjectSyncP.syncListP);
		connectionContext_packetSend(ctx, p);
		return;
	}

	sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
	protocol_blockSyncList(&p->packetTypeP.choice.dataObjectSyncP);
	
	connectionContext_syncOperand(ctx, sreq,
			&p->packetTypeP.choice.dataObjectSyncP.syncListP, NULL, sobj);

	connectionContext_packetSend(ctx, p);

#if 0
	if (sreq->childOp >= -1) {
		/* node operation */
next_childop:
		/* add child operation */
		if (sreq->childOp == -1) {
			syncOp = protocol_addChild();
			asn_sequence_add(&p->packetTypeP.choice.dataObjectSyncP.syncListP.
                    choice.blockSyncListP.list, syncOp);
			sreq->childOp = -2;
			citer = dataobject_childIterator(sobj);
			while (!listIterator_finished(citer)) {
				if (list_find(sreq->completeNodes, listIterator_item(citer), ListEqualComparitor) == NULL) {
					sreq->objectIndex = dataobject_treeIndex((DataObject *)listIterator_item(citer));
					break;
				}
				listIterator_next(citer);
			}
			listIterator_delete(citer);
		} else {
			/* go to tree operation */
			syncOp = protocol_goToTree(sreq->childOp);
			asn_sequence_add(&p->packetTypeP.choice.dataObjectSyncP.syncListP.
                    choice.blockSyncListP.list, syncOp);
			sreq->objectIndex = sreq->childOp;
			sreq->childOp = -1;
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			goto next_childop;
		}
		connectionContext_packetSend(ctx, p);
		return;
	}
	while (!mapIterator_finished(iter)) {
		field = (DataObjectField *)mapIterator_item(iter, (void **)&fieldName);
		if (list_find(sreq->completeFields, fieldName, (ListComparitor)strcmp) == NULL) {
			/* serialize field */
			syncOp = protocol_serializeField(sobj, fieldName);
			if (syncOp == NULL)
				return;
            /* add to packet and mark field complete */
            asn_sequence_add(&p->packetTypeP.choice.dataObjectSyncP.syncListP.
                    choice.blockSyncListP.list, syncOp);
			list_append(sreq->completeFields, fieldName);
		}
		mapIterator_next(iter);
	}
	list_append(sreq->completeNodes, sobj);
	if (sreq->completeFields != NULL) {
		list_delete(sreq->completeFields);
		sreq->completeFields = NULL;
	}
	if  (dataobject_getTreeNextOp(sobj, &childOp))
		sreq->childOp = childOp;
	else
		sreq->hasFinished = 1;
#endif
}

static void connectionContext_processSync(ConnectionContext *ctx,
		DataObjectSyncP_t *p)
{
	const char *mapKey;
	SyncRequest *sreq;
	DataObject *sobj;

	emo_printf("DataObjectSync packet" NL);
	mapKey = generate_mapKey(ctx->endpoint, p->syncSequenceIDP);
	sreq = map_find(ctx->syncRequests, mapKey);
	if (sreq == NULL) {
		emo_printf("recvd sync for request not in progress: %d" NL, 
				p->syncSequenceIDP);
		return;
	}	

	if (p->syncListP.present == SyncListP_PR_blockSyncListP) {
		sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
		if (sobj == NULL) {
			emo_printf("Current Packet at NULL position, abort" NL);
#ifdef SIMULATOR
			abort();
#endif
			return;
		}
		connectionContext_processBlockSyncList(ctx, sreq, sobj,
				&p->syncListP.choice.blockSyncListP);
	} else {
		connectionContext_processRecordSyncList(ctx, sreq,
				&p->syncListP.choice.recordSyncListP);
	}

}

static void connectionContext_processSyncOperand(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *sobj, SyncOperandP_t *syncOp)
{
	const char *fieldName;
	DataObjectField *dof;
	void *data;
	DataObject *cobj;

	fieldName = (char *)syncOp->fieldNameP.buf;
	/*emo_printf("Field is '%s'" NL, fieldName);*/
	if (syncOp->syncP.present == syncP_PR_syncSetP) {
		dof = dataobjectfield_string((const char *)syncOp->syncP.choice.syncSetP.buf);
		dataobject_setValue(sobj, fieldName, dof);
	} else if (syncOp->syncP.present == syncP_PR_syncModifyP) {
		dof = dataobject_getValue(sobj, fieldName);
		if (dof == NULL) {
			data = p_malloc(syncOp->syncP.choice.syncSetP.size);
			memcpy(data, syncOp->syncP.choice.syncSetP.buf,
					syncOp->syncP.choice.syncSetP.size);
			dof = dataobjectfield_data(data, syncOp->syncP.choice.syncSetP.size);
			dataobject_setValue(sobj, fieldName, dof);
		} else {
			/* we assume they are always doing an append atm */
			p_realloc(dof->field.data.bytes, dof->field.data.size+syncOp->syncP.choice.syncSetP.size);
			memcpy(dof->field.data.bytes+dof->field.data.size, syncOp->syncP.choice.syncSetP.buf,
					syncOp->syncP.choice.syncSetP.size);
			dof->field.data.size += syncOp->syncP.choice.syncSetP.size;
		}
	} else if (syncOp->syncP.present == syncP_PR_nodeOperationP) {
		if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeAddP) {
			if (syncOp->syncP.choice.nodeOperationP.choice.nodeAddP == nodeAddP_nodeChildP) {
				cobj = dataobject_new();
				dataobject_pack(sobj, cobj);
				sobj = cobj;
				sreq->objectIndex = dataobject_treeIndex(sobj);
				emo_printf("#### Add child - new index: %d" NL, sreq->objectIndex);
				//dataobject_debugPrint(sreq->dobj);
			} else {
				emo_printf("Unsuported node add type" NL);
			}
		} else if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeGotoTreeP) {
			sreq->objectIndex = syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP;
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			emo_printf("#### GoTo index: %d" NL, sreq->objectIndex);
			/*dataobject_debugPrint(sreq->dobj);*/
			if (sobj == NULL) {
				emo_printf("Going to invalid index" NL);
#ifdef SIMULATOR
				abort();
#endif
				return;
			}
		} else {
			emo_printf("Unsupported type of node operation" NL);
		}
	} else {
		emo_printf("Dont support that type of sync operation yet" NL);
	}

}

static void connectionContext_processBlockSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *sobj, struct blockSyncListP *p)
{
	int i;
	SyncOperandP_t *syncOp;

	for (i = 0; i < p->list.count; ++i) {
		syncOp = p->list.array[i];
		sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
		connectionContext_processSyncOperand(ctx, sreq, sobj, syncOp);
	}
}

static void connectionContext_processRecordSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, struct recordSyncListP *p)
{
	RecordSyncListP_t *rsl;
	ListIterator *iter;
	unsigned int minor, major;
	DataObject *robj;
	SyncOperandP_t *syncOp;
	int i, j;

	dataobject_setRecordType(sreq->dobj, 1);

	for (i = 0; i < p->list.count; ++i) {
		rsl = p->list.array[i];
		if (rsl->deleteRecordP) {
			emo_printf("Delete record not supported yet" NL);
			continue;
		}
		robj = NULL;
		for (iter = dataobject_childIterator(sreq->dobj);
				!listIterator_finished(iter); listIterator_next(iter)) {
			robj = listIterator_item(iter);
			dataobject_getStamp(robj, &minor, &major);
			if (minor == rsl->recordIdMinorP && major == rsl->recordIdMajorP) {
				robj = dataobject_getTree(robj, sreq->objectIndex);			
				break;
			}
			robj = NULL;
		}
		if (robj == NULL) {
			robj = dataobject_new();
			dataobject_setStamp(robj, rsl->recordIdMinorP, rsl->recordIdMajorP);
			dataobject_appendRecord(sreq->dobj, robj);
		}
		for (j = 0; j < rsl->recordFieldListP->list.count; ++j) {
			syncOp = rsl->recordFieldListP->list.array[j];
			connectionContext_processSyncOperand(ctx, sreq, robj, syncOp);
		}
	}
}

static void connectionContext_processAuthRequest(ConnectionContext *ctx,
		AuthRequestP_t *p)
{
	AuthTypeP_t authType;
	int i;

	emo_printf("Processing request auth" NL);

	for (i = 0; i < p->authTypesP.list.count; ++i) {
		authType = *p->authTypesP.list.array[i];
		emo_printf("Got authType request %d" NL, authType);
		if (authType == AuthTypeP_atImplicitP) {
			ctx->needAuth = NA_NO;
		} else {
			ctx->needAuth = NA_YES;
		}
	}
	
	connectionContext_authUserPass(ctx);
}

static void connectionContext_authUserPass(ConnectionContext *ctx)
{
	FRIPacketP_t packet;
	AuthUserPassP_t *p;
#ifndef SIMULATOR
	static char deviceImei[IMEI_LEN + 1] = { 0 };

	if (!deviceImei[0])
		BalGetImei(deviceImei);
#endif

	packet.packetTypeP.present = packetTypeP_PR_authUserPassP;
	p = &packet.packetTypeP.choice.authUserPassP;

	protocol_authUserPass(p, "peek", "peek123");

	/* add our extras */
#ifndef SIMULATOR
	protocol_autUserPassExtra(p, "IMEI", deviceImei);
#else
	protocol_autUserPassExtra(p, "IMEI", "312000012345678");
#endif
	protocol_autUserPassExtra(p, "IP", "192.168.1.1");

	emo_printf("Sending auth user pass" NL);
	connectionContext_packetSend(ctx, &packet);
}

static void connectionContext_processAuthUserPass(ConnectionContext *ctx,
		AuthUserPassP_t *p)
{
	FRIPacketP_t packet;

	emo_printf("Got Auth User Pass" NL);

	packet.packetTypeP.present = packetTypeP_PR_authResponseP;
	protocol_authResponse(&packet.packetTypeP.choice.authResponseP,
			RequestResponseP_responseOKP);

	emo_printf("Sending Auth OK" NL);
	ctx->hasAuth = 1;

	connectionContext_packetSend(ctx, &packet);
}

static void connectionContext_processAuthResponse(ConnectionContext *ctx,
		AuthResponseP_t *p)
{
	if (*p == RequestResponseP_responseOKP) {
		emo_printf("Auth OK" NL);
		ctx->hasAuth = 1;
	} else {
		emo_printf("Auth not OK with: %d" NL, *p);
	}
}

static void connectionContext_recordSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, SyncListP_t *p)
{
	ListIterator *iter;
	RecordSyncListP_t *rsync;
	int idx = 0;
	DataObject *sobj;

	for (iter = widget_getChildren(sreq->dobj); !listIterator_finished(iter);
			listIterator_next(iter)) {
		if (idx != sreq->recordIndex) {
			++idx;
			continue;
		}
		fprintf(stderr, "RecordSync for index %d - object index %d\n",
				idx, sreq->objectIndex);
		sobj = dataobject_getTree((DataObject *)listIterator_item(iter),
				sreq->objectIndex);
		rsync = connectionContext_recordSync(ctx, sreq, sobj);
		if (rsync != NULL)
			asn_sequence_add(&p->choice.recordSyncListP.list, rsync);
		if (sreq->hasFinished) {
			sreq->hasFinished = 0;
			sreq->objectIndex = 0;
			++sreq->recordIndex;
			list_delete(sreq->completeNodes);
			sreq->completeNodes = list_new();
		}
		break;
	}
	listIterator_delete(iter);

	if (sreq->recordIndex == dataobject_getChildCount(sreq->dobj))
		sreq->hasFinished = 1;
}

static RecordSyncListP_t *connectionContext_recordSync(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *dobj)
{
	RecordSyncListP_t *p;
	unsigned int stampMinor, stampMajor;

	dataobject_getStamp(dobj, &stampMinor, &stampMajor);
	p = protocol_recordSync(0, stampMinor, stampMajor);

	connectionContext_syncOperand(ctx, sreq, NULL, p, dobj);

	return p;
}

static void connectionContext_syncOperand(ConnectionContext *ctx,
		SyncRequest *sreq, SyncListP_t *list, RecordSyncListP_t *listr,
		DataObject *sobj)
{
	SyncOperandP_t *syncOp;
	int childOp;
	char *fieldName;
	ListIterator *citer;
	DataObjectField *field;
	MapIterator *iter;

	iter = dataobject_fieldIterator(sobj);

	if (sreq->childOp >= -1) {
		/* node operation */
next_childop:
		/* add child operation */
		if (sreq->childOp == -1) {
			/*emo_printf("Add Child" NL);*/
			syncOp = protocol_addChild();
			if (list != NULL)
				asn_sequence_add(&list->choice.blockSyncListP.list, syncOp);
			else
				asn_sequence_add(&listr->recordFieldListP->list, syncOp);

			sreq->childOp = -2;
			citer = dataobject_childIterator(sobj);
			while (!listIterator_finished(citer)) {
				if (list_find(sreq->completeNodes, listIterator_item(citer), ListEqualComparitor) == NULL) {
					sreq->objectIndex = dataobject_treeIndex((DataObject *)listIterator_item(citer));
					/*emo_printf("Node to go to %p with index %d" NL, listIterator_item(citer),
							sreq->objectIndex);
					list_debug(sreq->completeNodes);*/
					break;
				}
				listIterator_next(citer);
			}
			listIterator_delete(citer);
		} else {
			/* go to tree operation */
			/*emo_printf("GoTo Child %d" NL, sreq->childOp);*/
			syncOp = protocol_goToTree(sreq->childOp);
			if (list != NULL)
				asn_sequence_add(&list->choice.blockSyncListP.list, syncOp);
			else
				asn_sequence_add(&listr->recordFieldListP->list, syncOp);
			sreq->objectIndex = sreq->childOp;
			sreq->childOp = -1;
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			goto next_childop;
		}
		return;
	}
	while (!mapIterator_finished(iter)) {
		field = (DataObjectField *)mapIterator_item(iter, (void **)&fieldName);
		if (list_find(sreq->completeFields, fieldName, (ListComparitor)strcmp) == NULL) {
			/* serialize field */
			syncOp = protocol_serializeField(sobj, fieldName);
			if (syncOp == NULL)
				return ;
            /* add to packet and mark field complete */
			if (list != NULL)
				asn_sequence_add(&list->choice.blockSyncListP.list, syncOp);
			else
				asn_sequence_add(&listr->recordFieldListP->list, syncOp);
			list_append(sreq->completeFields, fieldName);
		}
		mapIterator_next(iter);
	}
	list_append(sreq->completeNodes, sobj);
	/*emo_printf("Node Complete %p" NL, sobj);
	list_debug(sreq->completeNodes);*/
	if (sreq->completeFields != NULL) {
		list_delete(sreq->completeFields);
		sreq->completeFields = NULL;
	}
	if (dataobject_getTreeNextOp(sobj, &childOp))
		sreq->childOp = childOp;
	else
		sreq->hasFinished = 1;
}