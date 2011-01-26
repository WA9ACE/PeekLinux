#include "ConnectionContext.h"

#include "Map.h"
#include "DataObject.h"
#include "List.h"
#include "Widget.h"
#include "Debug.h"
#include "Protocol.h"
#include "ProtocolUtils.h"
#include "Application.h"
#include "ApplicationManager.h"
#include "RenderManager.h"
#include "Cache.h"
#include "Mime.h"

#include "Style.h"

#include "p_malloc.h"

extern const char *EmoGetImei();
//extern void appProtocolStatus(int status);

static const int CCTX_BUFLEN = 4096;

struct SyncRequest_t {
	URL *url;
	int newObject;
	int isClient;
	int hasStarted;
	int hasSentLocal;
	int hasFinished;
	int remoteFinished;
	int finalize;
	RequestResponseP_t remoteResponse;
	unsigned int stampMinor;
	unsigned int stampMajor;
	
	DataObject *dobj;
	DataObject *forcedObject;
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
static void connectionContext_outgoingSyncForced(ConnectionContext *ctx,
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

	EMO_ASSERT_NULL(ep != NULL,
			"new connection context with NULL end point")

	map = map_string();
	if (map == NULL)
		return NULL;

	buffer = p_malloc(CCTX_BUFLEN);
	if (buffer == NULL) {
		map_delete(map);
		return NULL;
	}
	memset(buffer, 0, CCTX_BUFLEN);

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
	EMO_ASSERT(ctx != NULL,
			"delete connection context with NULL context")

	map_delete(ctx->syncRequests);
	p_free(ctx->buffer);
	p_free(ctx);
}

void connectionContext_setBuffer(ConnectionContext *ctx, char *buffer, int size) 
{
	EMO_ASSERT(ctx != NULL,
			"setbuffer connection context with NULL context")
	EMO_ASSERT(buffer != NULL,
			"setbuffer connection context buffer is NULL")

	emo_printf("connectionContext_setBuffer() buffer size %d", size);
        memcpy(ctx->buffer+ctx->bufferBytes, buffer, size); 
        ctx->bufferBytes += size;
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
	MapIterator iter;
	void *key;
	SyncRequest *sreq;

	EMO_ASSERT_INT(ctx != NULL, 0,
			"connection context loop iteration without context")
#if 0
	emo_printf("connectionContext_loopIteration()" NL);
#endif
#ifdef SIMULATOR
	transport = endpoint_getTransport(ctx->endpoint);

	/*emo_printf("Loop iteration...");*/

	/* first check if there is any inbound data pending on our connection */
	response = transport->peek(ctx->endpoint, ctx->buffer+ctx->bufferBytes, 1);
	if (response < 0) {
        /*emo_printf("transport->peek respose: %d\n", response);*/
		/* error state, or no data */
	} else if (response > 0) {
		response = transport->read(ctx->endpoint, ctx->buffer+ctx->bufferBytes, CCTX_BUFLEN-ctx->bufferBytes);
#ifndef SIMULATOR
		emo_printf("Read %d bytes" NL, response);
#endif
		if (response < 0) {
			emo_printf("read got error state" NL);
			/* error state */
		} else if (response > 0) {
			ctx->bufferBytes += response;
			do {
				response = connectionContext_consumePacket(ctx);
#ifndef SIMULATOR
				emo_printf("Consumed %d bytes" NL, response);
#endif
			} while (response > 0);
			return 1;
		}
	}
#endif
	if (!((ctx->needAuth == NA_YES && ctx->hasAuth) || ctx->needAuth == NA_NO)) {
		emo_printf("Not doing outgoing sync because we are not authorized" NL);
		return 0;
	}
#ifndef SIMULATOR
	emo_printf("Outgoing sync" NL);
#endif

	/* perform any outgoing sync requests */
	map_begin(ctx->syncRequests, &iter);
	while (!mapIterator_finished(&iter)) {
		sreq = (SyncRequest *)mapIterator_item(&iter, &key);

#ifndef SIMULATOR
		emo_printf("Processing sync request %s" NL, sreq->url->all);
#endif

		connectionContext_processSyncRequest(ctx, sreq);
		if (sreq->finalize)
			mapIterator_remove(&iter);
		else
			mapIterator_next(&iter);
		return 1;
	}

#ifndef SIMULATOR
	emo_printf("Outgoing sync complete" NL);
#endif

	return 0;
}

void connectionContext_requestAuth(ConnectionContext *ctx)
{
	FRIPacketP_t packet;
	AuthRequestP_t *p;
	AuthTypeP_t authType;

	EMO_ASSERT(ctx != NULL,
			"connection context reqeust auth without context")

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

int connectionContext_syncRequestForce(ConnectionContext *ctx, URL *url,
		DataObject *dobj)
{
	SyncRequest *sreq;
	char mapKey[64];
	DataObject *cobj;
	DataObjectField *field;

	EMO_ASSERT_INT(url != NULL, 0,
			"connection context forced reqeust missing URL")

	if (dobj == NULL) {
		cobj = cache_loadObject(url);
		if (cobj != NULL) {
			dataobject_exportGlobal(cobj, url, 0);
			dataobject_resolveReferences(cobj);
			mime_loadAll(cobj);

			field = dataobject_getEnum(cobj, EMO_FIELD_TYPE);
			if (EMO_DOF_IS_TYPE(field, EMO_TYPE_APPLICATION))
				manager_loadApplication(cobj, 1, url);
			return 1;
		}

#if defined(SIMULATOR) && !defined(LINUX)
		if (strcmp(url->scheme, "xml") == 0) {
			extern void xmlLoadObject(URL *url);
			xmlLoadObject(url);
			return 1;
		}
#endif
	}

	if (ctx == NULL) {
		emo_printf("ConnectionContext is NULL.." NL);
		return 0;
	}

	sreq = syncRequest_new(url, 1);
	sreq->hasStarted = 0;
	sreq->sequenceID = endpoint_getTransport(ctx->endpoint)->sequenceID(ctx->endpoint);
	sreq->forcedObject = dobj;

	strcpy(mapKey, generate_mapKey(ctx->endpoint, sreq->sequenceID));
	/*emo_printf("@New Sync Request mapKey: %s" NL, mapKey);*/
	map_append(ctx->syncRequests, mapKey, sreq);

	// TODO Evaluate whether this is the best place for the loop iteration
	connectionContext_loopIteration(ctx);
	return 1;
}

int connectionContext_syncRequest(ConnectionContext *ctx, URL *url)
{
	emo_printf("connectionContext_syncRequest()");
	return connectionContext_syncRequestForce(ctx, url, NULL);
}

int connectionContext_consumePacket(ConnectionContext *ctx)
{
	asn_dec_rval_t retval;
	FRIPacketP_t *packet = NULL;
	int consumed;

	EMO_ASSERT_INT(ctx != NULL, 0,
			"connection context consume packet without context")

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
#endif
	retval = uper_decode_complete(&ctx->decodeContext, &asn_DEF_FRIPacketP,
			(void **)&packet, ctx->buffer, ctx->bufferBytes);

#ifndef SIMULATOR
	emo_printf("connectionContext_consumePacket(): uper_decode_complete returned %d" NL, retval.code);
#endif
	switch (retval.code) {
		case RC_OK:
			consumed = retval.consumed;
			connectionContext_processPacket(ctx, packet);
			memmove(ctx->buffer, ctx->buffer + consumed,
					ctx->bufferBytes - consumed);
			ctx->bufferBytes -= consumed;
#ifndef SIMULATOR
			emo_printf("connectionContext_consumePacket() RC_OK finished" NL);
#endif
			/*protocolFreeFRIPacketP(packet);*/
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
	char *tmpstr, *foundstr;
	DataObject *dobj;
	SyncRequest *sreq;
	URL *url;
	int isLocal;
	const char *mapKey;

	EMO_ASSERT(ctx != NULL,
			"connection context process sync start without context")
	EMO_ASSERT(p != NULL,
			"connection context process sync start without packet input")

	emo_printf("SyncStart packet" NL);
	tmpstr = OCTET_STRING_to_string(&p->urlP);
	if (tmpstr == NULL) {
		emo_printf("Faild to get URL in sync start" NL);
		return;
	}
	foundstr = (char *)list_find(ctx->inprogressRequests, tmpstr, (ListComparitor)strcmp);
	emo_printf("SyncStart : %s" NL, tmpstr);
	if (foundstr != NULL) {
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
		emo_printf("Received a Sync request for non existing object :%s - creating it" NL,
				tmpstr);
		dobj = dataobject_construct(url, 0);
		/*url_delete(url);
		p_free(tmpstr);
		return;*/
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
		emo_printf("Setting inprogress: %s" NL, tmpstr);
		list_append(ctx->inprogressRequests, p_strdup(tmpstr));
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
	ListIterator iter;
	/*Application *app;*/

	EMO_ASSERT(ctx != NULL,
			"connection context process packet without context")
	EMO_ASSERT(packet != NULL,
			"connection context process packet without packet input")

#ifndef SIMULATOR
	emo_printf("Got packet : %d" NL, packet->packetTypeP.present);
#endif

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
			/*emo_printf("Map Key: %s" NL, mapKey);*/
			sreq = map_find(ctx->syncRequests, mapKey);
			if (sreq == NULL) {
				emo_printf("recvd sync finished for request not in progress: %d" NL, 
						packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
				break;
			}
			emo_printf("Finished %s" NL, sreq->url->all);
			//if (dataobject_getRecordType(sreq->dobj))
			//	dataobject_debugPrint(sreq->dobj);
			sreq->remoteResponse = packet->packetTypeP.choice.dataObjectSyncFinishP.responseP;
			sreq->remoteFinished = 1;
			if (sreq->isClient) {
				sreq->finalize = 1;
				dataobject_setState(sreq->dobj, DOS_OK);
				/*widget_resolveLayout(sreq->dobj, currentStyle);*/
				widget_markDirty(sreq->dobj);
				field = dataobject_getEnum(sreq->dobj, EMO_FIELD_TYPE);

				/* call onsyncfinish handlers */
				if (!sreq->newObject) {
					//dataobject_debugPrint(sreq->forcedObject);
					if (sreq->forcedObject != NULL)
						dataobject_onsyncfinished(sreq->forcedObject);
					else
						dataobject_onsyncfinished(sreq->dobj);
				}

				cache_commitServerSide(sreq->dobj, sreq->url);
				cache_commit();

				//dataobject_resolveReferences(sreq->dobj);

				dataobject_setState(sreq->dobj, DOS_OK);
				if (field != NULL && field->type == DOF_STRING)
					emo_printf("*** Finished Type: %s" NL, field->field.string);
				if (field != NULL && field->type == DOF_STRING &&
						EMO_DOF_IS_TYPE(field, EMO_TYPE_APPLICATION) == 0) {
					if (sreq->newObject)
						manager_loadApplication(sreq->dobj, 1, sreq->url);
					/*app = application_load(sreq->dobj);
					manager_launchApplication(app);
					manager_focusApplication(app);*/
					/*dataobject_debugPrint(sreq->dobj);*/
				} else {
					/* FIXME: dirty */
					/*renderman_clearQueue();*/
					dataobject_setIsModifiedTree(sreq->dobj, 1);
				}
	
				renderman_markLayoutChanged();
				renderman_clearQueue();
				/*dataobject_debugPrint(sreq->dobj);*/

				if (list_findIter(ctx->inprogressRequests, sreq->url->all,
						(ListComparitor)strcmp, &iter)) {
					//sreq = (SyncRequest *)listIterator_item(&iter);
					emo_printf("Removing %s" NL, sreq->url->all);
					listIterator_remove(&iter);
					/* FIXME: delete sreq */
				} /*else {
					emo_printf("Finished a request that wasnt in-progress" NL);
					emo_abort;
				}*/
				
				/*dataobject_debugPrint(sreq->dobj);*/
				
				/* else {
					manager_focusApplication(manager_getFocusedApplication());
				}*/
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

	EMO_ASSERT(ctx != NULL,
			"connection context process sync request without context")
	EMO_ASSERT(sreq != NULL,
			"connection context process sync request without request")

	transport = endpoint_getTransport(ctx->endpoint);

	if (sreq->isClient) {
		emo_printf("Req %s - %d, %d, %d" NL, sreq->url->all,
				sreq->hasStarted, sreq->hasSentLocal, sreq->hasFinished);
		if (!sreq->hasStarted) {
			/* send start packet */
			/* we dont have this object at all so use stamp 0 */
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncStartP;
			protocol_syncStart(&packet.packetTypeP.choice.dataObjectSyncStartP,
					sreq->url->all, 0, 0, sreq->sequenceID);
			emo_printf("Adding inprogredd %s" NL, sreq->url->all);
			list_append(ctx->inprogressRequests, p_strdup(sreq->url->all));
			emo_printf( "Sending SyncStart" NL);
			connectionContext_packetSend(ctx, &packet);
			dataobject_setState(sreq->dobj, DOS_SYNC);
			sreq->hasStarted = 1;
			return;
		}
		if (!sreq->hasSentLocal) {
			/* this is our first sync of this object so we dont have anything
				to send back to the server */
			if (!sreq->newObject && sreq->forcedObject != NULL)
				connectionContext_outgoingSyncForced(ctx, sreq, &packet);
			sreq->hasSentLocal = 1;
		}
		if (!sreq->hasFinished) {
			/* for now, send a 'finished' packet since we dont send our changes */
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
			protocol_syncFinished(&packet.packetTypeP.choice.dataObjectSyncFinishP,
					RequestResponseP_responseExpiredP, sreq->sequenceID);
			emo_printf( "Sending SyncFinish for %s, %d" NL, sreq->url->all, sreq->sequenceID);
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

	EMO_ASSERT(ctx != NULL,
			"connection context packet send without context")
	EMO_ASSERT(packet != NULL,
			"connection context packet send missing packet")

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
#if 0
	if (retval != size)
		emo_printf("Sent %d bytes, wanted to send %d" NL, retval, size);
#endif
	p_free(buffer);
	protocolFreeFRIPacketP_children(packet);
}

static SyncRequest *syncRequest_new(URL *url, int isClient)
{
	SyncRequest *output;

	EMO_ASSERT_NULL(url != NULL,
			"connection context sync reqeust new with NULL url")

	output = p_malloc(sizeof(SyncRequest));
	output->url = url_parse(url->all, URL_ALL);
	output->isClient = isClient;
	output->hasFinished = 0;
	output->hasSentLocal = 0;
	output->remoteFinished = 0;
	output->finalize = 0;
	output->objectIndex = 0;
	output->recordIndex = 0;
	output->completeFields = NULL;
	output->completeNodes = list_new();
	output->dobj = dataobject_locate(url);
	output->childOp = -2;
	output->newObject = 0;
	output->forcedObject = NULL;
	if (output->dobj == NULL) {
		output->dobj = dataobject_construct(url, !isClient);
		output->newObject = 1;
	}
	dataobject_setState(output->dobj, DOS_SYNC);

	return output;
}

static void syncRqeuest_delete(SyncRequest *sreq)
{
	EMO_ASSERT(sreq != NULL,
			"connection context delete sync reqeust on NULL")

	p_free(sreq);
}

static void connectionContext_outgoingSync(ConnectionContext *ctx,
		SyncRequest *sreq, FRIPacketP_t *p)
{
	DataObject *sobj;
	
	EMO_ASSERT(ctx != NULL,
			"connection context outgoing sync without context")
	EMO_ASSERT(sreq != NULL,
			"connection context outgoing sync missing request")
	EMO_ASSERT(p != NULL,
			"connection context outgoing sync missing packet")

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
}

static void connectionContext_outgoingSyncForced(ConnectionContext *ctx,
		SyncRequest *sreq, FRIPacketP_t *p)
{
	DataObject *sobj;

	EMO_ASSERT(ctx != NULL,
			"connection context outgoing sync forced without context")
	EMO_ASSERT(sreq != NULL,
			"connection context outgoing sync forced missing request")
	EMO_ASSERT(p != NULL,
			"connection context outgoing sync forced missing packet")

	emo_printf("Sending SyncForce Packet" NL);
    p->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	if (sreq->completeFields == NULL)
		sreq->completeFields = list_new();

	p->packetTypeP.choice.dataObjectSyncP.syncSequenceIDP = sreq->sequenceID;

	sobj = sreq->forcedObject;
	sreq->objectIndex = dataobject_treeIndex(sobj);
	/*sobj = dataobject_getForcedObject(sreq->dobj, &sreq->objectIndex);*/
	if (sobj == NULL) {
		emo_printf("No forced object" NL);
		return;
	}
	sreq->childOp = sreq->objectIndex;
	protocol_blockSyncList(&p->packetTypeP.choice.dataObjectSyncP);
	
	connectionContext_syncOperand(ctx, sreq,
			&p->packetTypeP.choice.dataObjectSyncP.syncListP, NULL, sobj);

	connectionContext_packetSend(ctx, p);
}

static void connectionContext_processSync(ConnectionContext *ctx,
		DataObjectSyncP_t *p)
{
	const char *mapKey;
	SyncRequest *sreq;
	DataObject *sobj;

	EMO_ASSERT(ctx != NULL,
			"connection context process sync without context")
	EMO_ASSERT(p != NULL,
			"connection context process sync missing packet")
#ifndef SIMULATOR
	emo_printf("DataObjectSync packet" NL);
#endif
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
	const char *fieldName = NULL;
	EmoField fieldEnum = -1;
	DataObjectField *dof;
	void *data;
	DataObject *cobj;

	EMO_ASSERT(ctx != NULL,
			"connection context process sync operand without context")
	EMO_ASSERT(sreq != NULL,
			"connection context process sync operand without request")
	EMO_ASSERT(sobj != NULL,
			"connection context process sync operand without DataObject")
	EMO_ASSERT(syncOp != NULL,
			"connection context process sync operand missing operand")

	// PROTOCOLFIX - needs to handle enum
	if (syncOp->fieldNameP.present == FieldNameP_PR_fieldNameStringP)
		fieldName = (char *)syncOp->fieldNameP.choice.fieldNameStringP.buf;
	else
		fieldEnum = syncOp->fieldNameP.choice.fieldNameEnumP;
	/*emo_printf("Field is '%s'" NL, fieldName);*/
	if (syncOp->syncP.present == syncP_PR_syncSetP) {
		dof = dataobjectfield_string((const char *)syncOp->syncP.choice.syncSetP.buf);
		if (fieldEnum == -1)
			dataobject_setValue(sobj, fieldName, dof);
		else
			dataobject_setEnum(sobj, fieldEnum, dof);
	} else if (syncOp->syncP.present == syncP_PR_syncModifyP) {
		if (fieldName != NULL)
			dof = dataobject_getValue(sobj, fieldName);
		else
			dof = dataobject_getEnum(sobj, fieldEnum);
		if (dof == NULL) {
			data = p_malloc(syncOp->syncP.choice.syncSetP.size);
			memcpy(data, syncOp->syncP.choice.syncSetP.buf,
					syncOp->syncP.choice.syncSetP.size);
			dof = dataobjectfield_data(data, syncOp->syncP.choice.syncSetP.size);
			if (fieldEnum == -1)
				dataobject_setValue(sobj, fieldName, dof);
			else
				dataobject_setEnum(sobj, fieldEnum, dof);
		} else {
			if (dof->type == DOF_DATA) {
				/* we assume they are always doing an append atm */
				dof->field.data.bytes = (unsigned char *)p_realloc(dof->field.data.bytes,
						dof->field.data.size+syncOp->syncP.choice.syncSetP.size);
				memcpy(dof->field.data.bytes+dof->field.data.size, syncOp->syncP.choice.syncSetP.buf,
						syncOp->syncP.choice.syncSetP.size);
				dof->field.data.size += syncOp->syncP.choice.syncSetP.size;
			} else {
				emo_printf("Trying to append data to a non-data field" NL);
#ifdef SIMULATOR
				abort();
#endif
			}
		}
	} else if (syncOp->syncP.present == syncP_PR_nodeOperationP) {
		if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeAddP) {
			if (syncOp->syncP.choice.nodeOperationP.choice.nodeAddP == nodeAddP_nodeChildP) {
				cobj = dataobject_new();
				dataobject_pack(sobj, cobj);
				sobj = cobj;
				sreq->objectIndex = dataobject_treeIndex(sobj);
				/*emo_printf("#### Add child - new index: %d" NL, sreq->objectIndex);*/
				/*dataobject_debugPrint(sreq->dobj);*/
			} else {
				emo_printf("Unsuported node add type" NL);
			}
		} else if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeGotoTreeP) {
			sreq->objectIndex = syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP;
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			/*emo_printf("#### GoTo index: %d" NL, sreq->objectIndex);*/
			/*dataobject_debugPrint(sreq->dobj);*/
			if (sobj == NULL) {
				emo_printf("Going to invalid index" NL);
#ifdef SIMULATOR
				abort();
#endif
				return;
			}
		} else if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeGotoNamedTreeP) {
			sobj = dataobject_findByName(sreq->dobj, (const char *)syncOp->syncP.choice.nodeOperationP.choice.nodeGotoNamedTreeP.buf);
			/*emo_printf("#### GoTo index: %d" NL, sreq->objectIndex);*/
			/*dataobject_debugPrint(sreq->dobj);*/
			if (sobj == NULL) {
				emo_printf("Going to invalid index" NL);
#ifdef SIMULATOR
				abort();
#endif
				return;
			}
			sreq->objectIndex = dataobject_treeIndex(sobj);
		} else {
			emo_printf("Unsupported type of node operation" NL);
#ifdef SIMULATOR
				abort();
#endif
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

	EMO_ASSERT(ctx != NULL,
			"connection context process block list without context")
	EMO_ASSERT(sreq != NULL,
			"connection context process block list without request")
	EMO_ASSERT(sobj != NULL,
			"connection context process block list without DataObject")
	EMO_ASSERT(p != NULL,
			"connection context process block list missing list")

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
	ListIterator iter;
	unsigned int minor, major;
	DataObject *robj;
	SyncOperandP_t *syncOp;
	int i, j;

	EMO_ASSERT(ctx != NULL,
			"connection context process record list without context")
	EMO_ASSERT(sreq != NULL,
			"connection context process record list without request")
	EMO_ASSERT(p != NULL,
			"connection context process record list missing list")

	dataobject_setRecordType(sreq->dobj, 1);

	for (i = 0; i < p->list.count; ++i) {
		rsl = p->list.array[i];
		robj = NULL;
		for (dataobject_childIterator(sreq->dobj, &iter);
				!listIterator_finished(&iter); listIterator_next(&iter)) {
			robj = listIterator_item(&iter);
			dataobject_getStamp(robj, &minor, &major);
			if (minor == (unsigned int)rsl->recordIdMinorP &&
						major == (unsigned int)rsl->recordIdMajorP) {
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
		if (rsl->deleteRecordP && !listIterator_finished(&iter)) {
			emo_printf("Deleting record" NL);
			listIterator_remove(&iter);
			dataobject_delete(robj);
			//dataobject_recordDelete(sreg->dobj, robj);
		}
	}
}

static void connectionContext_processAuthRequest(ConnectionContext *ctx,
		AuthRequestP_t *p)
{
	AuthTypeP_t authType;
	int i;

	EMO_ASSERT(ctx != NULL,
			"connection context process auth request without context")
	EMO_ASSERT(p != NULL,
			"connection context process auth request missing request")

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

	EMO_ASSERT(ctx != NULL,
			"connection context auth user/pass without context")

		packet.packetTypeP.present = packetTypeP_PR_authUserPassP;
	p = &packet.packetTypeP.choice.authUserPassP;

	protocol_authUserPass(p, "peek", "peek123");

	/* add our extras */
#ifndef SIMULATOR
	protocol_autUserPassExtra(p, "IMEI", EmoGetImei());
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

	EMO_ASSERT(ctx != NULL,
			"connection context process auth user/pass without context")
	EMO_ASSERT(p != NULL,
			"connection context process auth user/pass without packet")

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
	EMO_ASSERT(ctx != NULL,
			"connection context process auth response without context")
	EMO_ASSERT(p != NULL,
			"connection context process auth response without packet")

	if (*p == RequestResponseP_responseOKP) {
		emo_printf("Auth OK" NL);
		ctx->hasAuth = 1;
#ifndef SIMULATOR
		gprs_set_emobiix_on(1);
#endif
	} else {
		emo_printf("Auth not OK with: %d" NL, *p);
	}
}

static void connectionContext_recordSyncList(ConnectionContext *ctx,
		SyncRequest *sreq, SyncListP_t *p)
{
	ListIterator iter;
	RecordSyncListP_t *rsync;
	int idx = 0;
	DataObject *sobj;

	EMO_ASSERT(ctx != NULL,
			"connection context record sync list without context")
	EMO_ASSERT(sreq != NULL,
			"connection context record sync list without request")
	EMO_ASSERT(p != NULL,
			"connection context record sync list without packet")

	for (dataobject_childIterator(sreq->dobj, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		if (idx != sreq->recordIndex) {
			++idx;
			continue;
		}
		emo_printf("RecordSync for index %d - object index %d\n", idx, sreq->objectIndex);
		sobj = dataobject_getTree((DataObject *)listIterator_item(&iter),
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
	/*listIterator_delete(iter);*/

	if (sreq->recordIndex == dataobject_getChildCount(sreq->dobj))
		sreq->hasFinished = 1;
}

static RecordSyncListP_t *connectionContext_recordSync(ConnectionContext *ctx,
		SyncRequest *sreq, DataObject *dobj)
{
	RecordSyncListP_t *p;
	unsigned int stampMinor, stampMajor;

	EMO_ASSERT_NULL(ctx != NULL,
			"connection context record sync without context")
	EMO_ASSERT_NULL(sreq != NULL,
			"connection context record sync without request")
	EMO_ASSERT_NULL(dobj != NULL,
			"connection context record sync without DataObject")

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
	ListIterator citer;
	DataObjectField *field;
	MapIterator iter;

	EMO_ASSERT(ctx != NULL,
			"connection context sync operand without context")
	EMO_ASSERT(sreq != NULL,
			"connection context sync operand without request")
	EMO_ASSERT(sobj != NULL,
			"connection context sync operand without DataObject")

	dataobject_fieldIterator(sobj, &iter);

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
			dataobject_childIterator(sobj, &citer);
			while (!listIterator_finished(&citer)) {
				if (list_find(sreq->completeNodes, listIterator_item(&citer), ListEqualComparitor) == NULL) {
					sreq->objectIndex = dataobject_treeIndex((DataObject *)listIterator_item(&citer));
					/*emo_printf("Node to go to %p with index %d" NL, listIterator_item(citer),
							sreq->objectIndex);
					list_debug(sreq->completeNodes);*/
					break;
				}
				listIterator_next(&citer);
			}
			/*listIterator_delete(citer);*/
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
			if (sreq->newObject)
				goto next_childop;
		}
		/*return;*/
	}
	while (!mapIterator_finished(&iter)) {
		field = (DataObjectField *)mapIterator_item(&iter, (void **)&fieldName);
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
		mapIterator_next(&iter);
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
	else {
		if(sreq->forcedObject == NULL)
			sreq->hasFinished = 1;
	}
}

