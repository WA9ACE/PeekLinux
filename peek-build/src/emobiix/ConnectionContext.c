#include "ConnectionContext.h"



#include "Map.h"
#include "DataObject.h"
#include "List.h"
#include "Widget.h"
#include "Debug.h"

#include "Style.h"
extern Style *currentStyle;

#include "FRIPacketP.h"
#include "SyncOperandP.h"
#include "ProtocolUtils.h"

#include "p_malloc.h"


#include <asn_application.h>

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
};

static void connectionContext_processPacket(ConnectionContext *ctx,
		FRIPacketP_t *packet);
static void connectionContext_syncPacket(ConnectionContext *ctx,
		DataObjectSyncP_t *packet);
static void connectionContext_processSyncRequest(ConnectionContext *ctx,
		SyncRequest *sreq);
static void connectionContext_packetSend(ConnectionContext *ctx,
		FRIPacketP_t *packet);

/*static int encode_out(const void *buffer, size_t size, ConnectionContext *ctx)
{
	Transport *transport;

	emo_printf( "Sending %d bytes" NL, size);
	transport = endpoint_getTransport(ctx->endpoint);
	return (transport->write(ctx->endpoint, buffer, size) == size) ? 0 : -1;
}*/

static char *OCTET_STRING_to_string(OCTET_STRING_t *o)
{
	char *tmpstr;

	tmpstr = p_malloc(o->size+1);
	if (tmpstr == NULL)
		return NULL;
	tmpstr[o->size] = 0;
	memcpy(tmpstr, o->buf, o->size);

	return tmpstr;
}

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
	response = transport->peek(ctx->endpoint, ctx->buffer+ctx->bufferBytes,
			1);
	if (response < 0) {
        /*emo_printf("transport->peek respose: %d\n", response);*/
		/* error state, or no data */
	} else if (response > 0) {
#ifndef SIMULATOR
        ctx->bufferBytes += response;
#endif
		response = transport->read(ctx->endpoint, ctx->buffer+ctx->bufferBytes,
				CCTX_BUFLEN-ctx->bufferBytes);
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

	/*emo_printf("Outgoing sync");*/

	/* perform any outgoing sync requests */
	iter = map_begin(ctx->syncRequests);
	while (!mapIterator_finished(iter)) {
		sreq = (SyncRequest *)mapIterator_item(iter, &key);
        	emo_printf("Processing sync request %s" NL, sreq->url->all);
		connectionContext_processSyncRequest(ctx, sreq);
		if (sreq->finalize)
			mapIterator_remove(iter);
		else
			mapIterator_next(iter);
	}

	/*emo_printf("Outgoing sync complete");*/

	return 0;
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
	emo_printf("mapKey: %s" NL, mapKey);
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

static void connectionContext_processPacket(ConnectionContext *ctx,
		FRIPacketP_t *packet)
{
	SyncRequest *sreq;
	URL *url;
	int isLocal, i;
	char *tmpstr, *fieldName;
	DataObject *dobj, *sobj, *cobj;
	DataObjectField *dof;
	char mapKey[64];
	void *data;
	SyncOperandP_t *syncOp;
	/*unsigned int stampMinor, stampMajor;*/

	emo_printf("Got packet : %d" NL, packet->packetTypeP.present);

	switch (packet->packetTypeP.present) {
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
			emo_printf("SyncStart packet" NL);
			tmpstr = OCTET_STRING_to_string(&packet->packetTypeP.choice.dataObjectSyncStartP.urlP);
			if (tmpstr == NULL)
				break;
			sreq = (SyncRequest *)list_find(ctx->inprogressRequests, tmpstr, (ListComparitor)strcmp);
			emo_printf("SyncStart : %s" NL, tmpstr);
			if (sreq != NULL) {
				emo_printf("Received a Sync request for a already in progress object :%s" NL,
						tmpstr);
				break;
			}
			url = url_parse(tmpstr, URL_ALL);
			if (url == NULL) {
				p_free(tmpstr);
				break;
			}
			dobj = dataobject_locate(url);
			if (dobj == NULL) {
				emo_printf("Received a Sync request for non existing object :%s" NL,
						tmpstr);
				url_delete(url);
				break;
			}
			dataobject_setStamp(dobj, 0, 0);
			isLocal = dataobject_isLocal(dobj);
			sreq = syncRequest_new(url, !isLocal);
			if (sreq != NULL) {
				sreq->sequenceID = packet->packetTypeP.choice.dataObjectSyncStartP.syncSequenceIDP;
				sreq->dobj = dobj;
				sreq->hasStarted = 1;
				sreq->stampMinor = 
						packet->packetTypeP.choice.dataObjectSyncStartP.dataObjectStampMinorP;
				sreq->stampMajor = 
						packet->packetTypeP.choice.dataObjectSyncStartP.dataObjectStampMinorP;
				snprintf(mapKey, 64, "%p,%d", ctx->endpoint, sreq->sequenceID);
				emo_printf("mapKey: %s" NL, mapKey);
				map_append(ctx->syncRequests, mapKey, sreq);
				list_append(ctx->inprogressRequests, tmpstr);
			} else {
				url_delete(url);
				p_free(tmpstr);
			}
			break;
		case packetTypeP_PR_dataObjectSyncP:
			emo_printf("DataObjectSync packet" NL);
			snprintf(mapKey, 64, "%p,%d", ctx->endpoint, packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
			emo_printf("mapKey: %s" NL, mapKey);
			sreq = map_find(ctx->syncRequests, mapKey);
			if (sreq == NULL) {
				emo_printf("recvd sync for request not in progress: %d" NL, 
						packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
				break;
			}
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			if (sobj == NULL) {
				emo_printf("Current Packet at NULL position, abort" NL);
#ifdef SIMULATOR
				abort();
#endif
				return;
			}
			if (packet->packetTypeP.choice.dataObjectSyncP.syncListP.present ==
					SyncListP_PR_blockSyncListP) {
				for (i = 0; i < packet->packetTypeP.choice.dataObjectSyncP.
						syncListP.choice.blockSyncListP.list.count; ++i) {
					syncOp = packet->packetTypeP.choice.dataObjectSyncP.
						syncListP.choice.blockSyncListP.list.array[i];
					fieldName = (char *)syncOp->fieldNameP.buf;
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
			} else {
				emo_printf("Dont support that type of sync list yet" NL);
			}
			break;
		case packetTypeP_PR_dataObjectSyncFinishP:
			emo_printf( "DataObjectSyncFinish packet" NL);
			snprintf(mapKey, 64, "%p,%d", ctx->endpoint, packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP);
			emo_printf("mapKey: %s" NL, mapKey);
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
				widget_resolveLayout(sreq->dobj, currentStyle);
				widget_markDirty(sreq->dobj);
			}
			break;
		case packetTypeP_PR_NOTHING:
		default:
			emo_printf("Unknown or NONE packet recieved" NL);
	}
}
static void connectionContext_syncPacket(ConnectionContext *ctx,
		DataObjectSyncP_t *packet)
{

}

static void connectionContext_processSyncRequest(ConnectionContext *ctx,
		SyncRequest *sreq)
{
	FRIPacketP_t packet;
	/*unsigned int stampMinor, stampMajor;*/
	MapIterator *iter;
	ListIterator *citer;
	DataObjectField *field;
	char *fieldName;
	Transport *transport;
    SyncOperandP_t *syncOp;
	char mapKey[64];
	DataObject *sobj;
	int childOp;

	transport = endpoint_getTransport(ctx->endpoint);

	if (sreq->isClient) {
		if (!sreq->hasStarted) {
			/* send start packet */
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncStartP;
			packet.packetTypeP.choice.dataObjectSyncStartP.urlP.buf = (uint8_t *)sreq->url->all;
			packet.packetTypeP.choice.dataObjectSyncStartP.urlP.size = strlen(sreq->url->all);
			/* we dont have this object at all so use stamp 0 */
			packet.packetTypeP.choice.dataObjectSyncStartP.dataObjectStampMinorP = 0;
			packet.packetTypeP.choice.dataObjectSyncStartP.dataObjectStampMajorP = 0;
			packet.packetTypeP.choice.dataObjectSyncStartP.syncSequenceIDP = sreq->sequenceID;
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
			packet.packetTypeP.choice.dataObjectSyncFinishP.responseP =
					RequestResponseP_responseExpiredP;
			packet.packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP =
					sreq->sequenceID;
			emo_printf( "Sending SyncFinish" NL);
			connectionContext_packetSend(ctx, &packet);
			sreq->hasFinished = 1;
			return;
		}
	} else {
		if (sreq->hasStarted && sreq->remoteFinished && !sreq->hasFinished) {
			/* client synched to us, sync back to them */
			emo_printf( "Sending Sync Packet" NL);
            packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
			if (sreq->completeFields == NULL)
				sreq->completeFields = list_new();
			sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
			/*if (sobj == NULL) {
				sreq->hasFinished = 1;
				goto skip_sync;
				return;
			}*/
			iter = dataobject_fieldIterator(sobj);
            packet.packetTypeP.choice.dataObjectSyncP.syncListP.present =
                    SyncListP_PR_blockSyncListP;
            packet.packetTypeP.choice.dataObjectSyncP.syncListP.
                        choice.blockSyncListP.list.array = NULL;
            packet.packetTypeP.choice.dataObjectSyncP.syncListP.
                        choice.blockSyncListP.list.size = 0;
            packet.packetTypeP.choice.dataObjectSyncP.syncListP.
                        choice.blockSyncListP.list.count = 0;
			packet.packetTypeP.choice.dataObjectSyncP.syncListP.
                        choice.blockSyncListP.list.free = NULL;
			packet.packetTypeP.choice.dataObjectSyncP.syncSequenceIDP = sreq->sequenceID;
			if (sreq->childOp >= -1) {
next_childop:
				syncOp = (SyncOperandP_t *)p_malloc(sizeof(SyncOperandP_t));
				syncOp->fieldNameP.buf = NULL;
				OCTET_STRING_fromString(&syncOp->fieldNameP, "");
				syncOp->syncP.present = syncP_PR_nodeOperationP;
				if (sreq->childOp == -1) {
					syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeAddP;
					syncOp->syncP.choice.nodeOperationP.choice.nodeAddP = nodeAddP_nodeChildP;
					asn_sequence_add(&packet.packetTypeP.choice.dataObjectSyncP.syncListP.
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
					syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeGotoTreeP;
					syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP = sreq->childOp;
					asn_sequence_add(&packet.packetTypeP.choice.dataObjectSyncP.syncListP.
                            choice.blockSyncListP.list, syncOp);
					sreq->objectIndex = sreq->childOp;
					sreq->childOp = -1;
					sobj = dataobject_getTree(sreq->dobj, sreq->objectIndex);
					goto next_childop;
				}
				connectionContext_packetSend(ctx, &packet);
				return;
			}
			while (!mapIterator_finished(iter)) {
				field = (DataObjectField *)mapIterator_item(iter, (void **)&fieldName);
				if (list_find(sreq->completeFields, fieldName, (ListComparitor)strcmp) == NULL) {
					/* serialize field */
                    syncOp = (SyncOperandP_t *)p_malloc(sizeof(SyncOperandP_t));
					syncOp->fieldNameP.buf = NULL;
					OCTET_STRING_fromString(&syncOp->fieldNameP, fieldName);
                    field = dataobject_getValue(sobj, fieldName);
					if (field->type == DOF_STRING) {
						syncOp->syncP.present = syncP_PR_syncSetP;
						syncOp->syncP.choice.syncSetP.buf = NULL;
                        OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncSetP,
								field->field.string, strlen(field->field.string)+1);
					} else if (field->type == DOF_DATA) {
						syncOp->syncP.present = syncP_PR_syncModifyP;
						syncOp->syncP.choice.syncModifyP.modifyDataP.buf = NULL;
                        OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncModifyP.modifyDataP, 
                                (const char *)field->field.data.bytes,
                                field->field.data.size);
						syncOp->syncP.choice.syncModifyP.modifySizeP = field->field.data.size;
						syncOp->syncP.choice.syncModifyP.modifyOffsetP = 0;
					} else
                        emo_printf("Unsupported field type in sync" NL);

                    /* add to packet and mark field complete */
                    asn_sequence_add(&packet.packetTypeP.choice.dataObjectSyncP.syncListP.
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
			connectionContext_packetSend(ctx, &packet);
			return;
		}
/*skip_sync:*/
		if (sreq->hasStarted && sreq->remoteFinished && sreq->hasFinished) {
			/* for now, send a 'finished' and remove request */
			emo_printf( "Sending Finish Packet" NL);
			packet.packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
			packet.packetTypeP.choice.dataObjectSyncFinishP.responseP =
					RequestResponseP_responseOKP;
			packet.packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP =
					sreq->sequenceID;
			connectionContext_packetSend(ctx, &packet);
			snprintf(mapKey, 64, "%p,%d", ctx->endpoint, sreq->sequenceID);
			emo_printf("mapKey: %s" NL, mapKey);
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
