#include "Cache.h"

#include "Platform.h"
#include "p_malloc.h"
#include "URL.h"
#include "Array.h"
#include "File.h"
#include "Debug.h"
#include "SyncListP.h"
#include "Protocol.h"

#include <string.h>

#define CACHE_DISABLE

static DataObject *cacheManager = NULL;
/*
	<CacheManager>
		<authority url="server:port" index="0">
			<resource url="/calc" index="0" />
		</server>
	</CacheManager>
*/
static DataObject *load_manager(void);
static int save_manager(DataObject *mgr);
static int cache_writeObjectR(SyncListP_t *p, DataObject *dobj, File *rcFile, int *index, int modifiedOnly);
static int cache_readObjectR(SyncListP_t *p, DataObject *root, File *rcFile, int *index);

int cache_commitIndexs(URL *url, int *serverIndex, int *resourceIndex, int create)
{
	DataObject *sobj, *robj;
	ListIterator iter;
	DataObjectField *field, *indexField;
	int index, maxIndex;

	EMO_ASSERT_INT(cacheManager != NULL, 1, "commiting indexis to cache without manager")
	EMO_ASSERT_INT(serverIndex != NULL, 1,  "commiting object to cache without serverindex")
	EMO_ASSERT_INT(resourceIndex != NULL, 1, "commiting object to cache without resourceindex")

	index = 0;
	maxIndex = 0;
	sobj = NULL;
	for (dataobject_childIterator(cacheManager, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		sobj = (DataObject *)listIterator_item(&iter);
		field = dataobject_getEnum(sobj, EMO_FIELD_URL);
		indexField = dataobject_getEnumAsInt(sobj, EMO_FIELD_INDEX);
		if (indexField != NULL)
			index = indexField->field.integer;
		else
			emo_printf("Cache index field missing for authority" NL);
		if (dataobjectfield_isString(field, url->authority))
			break;
		sobj = NULL;
		if (index > maxIndex)
			maxIndex = index;
	}

	if (sobj == NULL) {
		if (!create)
			return 1;
		emo_printf("Cache record does not exist for authorty" NL);
		index = maxIndex + 1;
		sobj = dataobject_new();
		dataobject_setEnum(sobj, EMO_FIELD_TYPE, dataobjectfield_string("authority"));
		dataobject_setEnum(sobj, EMO_FIELD_URL, dataobjectfield_string(url->authority));
		dataobject_setEnum(sobj, EMO_FIELD_INDEX, dataobjectfield_int(index));
		dataobject_pack(cacheManager, sobj);
	}
	*serverIndex = index;

	index = 0;
	maxIndex = 0;
	robj = NULL;
	for (dataobject_childIterator(sobj, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		robj = (DataObject *)listIterator_item(&iter);
		field = dataobject_getEnum(robj, EMO_FIELD_URL);
		indexField = dataobject_getEnumAsInt(robj, EMO_FIELD_INDEX);
		if (indexField != NULL)
			index = indexField->field.integer;
		else
			emo_printf("Cache index field missing for path" NL);
		if (dataobjectfield_isString(field, url->path))
			break;
		robj = NULL;
		if (index > maxIndex)
			maxIndex = index;
	}

	if (robj == NULL) {
		if (!create)
			return 1;
		emo_printf("Cache record does not exist for path" NL);
		index = maxIndex + 1;
		robj = dataobject_new();
		dataobject_setEnum(robj, EMO_FIELD_TYPE, dataobjectfield_string("path"));
		dataobject_setEnum(robj, EMO_FIELD_URL, dataobjectfield_string(url->path));
		dataobject_setEnum(robj, EMO_FIELD_INDEX, dataobjectfield_int(index));
		dataobject_pack(sobj, robj);
	}
	*resourceIndex = index;

	return 0;
}

void cache_init(void)
{
#ifdef CACHE_DISABLE
	return;
#endif
	if (cacheManager != NULL)
		return;
	file_mkdir("cache");

	cacheManager = load_manager();
}

static int cache_encode_out(const void *buffer, size_t size, File *cFile)
{
	/*emo_printf("Cache writing %d bytes" NL, size);*/
	return (file_write(cFile, size, buffer) == size) ? 0 : -1;
}

int cache_writeObject(DataObject *dobj, File *rcFile, int modifiedOnly)
{
	SyncListP_t p;
	int index, result;
	asn_enc_rval_t retval;

#ifdef CACHE_DISABLE
	return 0;
#endif

	index = 0;
	memset(&p, 0, sizeof(SyncListP_t));
	p.present = SyncListP_PR_blockSyncListP;
	result = cache_writeObjectR(&p, dobj, rcFile, &index, modifiedOnly);
	retval = uper_encode(&asn_DEF_SyncListP, (void *)&p,
			(asn_app_consume_bytes_f *)cache_encode_out, (void *)rcFile);

	return result;
}

static int cache_writeObjectR(SyncListP_t *p, DataObject *dobj, File *rcFile, int *index, int modifiedOnly)
{
	ListIterator iter;
	MapIterator fiter;
	DataObjectField *field;
	DataObject *child;
	char *key;
	SyncOperandP_t *syncOp;
	int startIdx, loop;

	/*emo_printf("New serialize object" NL);*/

	for (dataobject_fieldIterator(dobj, &fiter); !mapIterator_finished(&fiter);
			mapIterator_next(&fiter)) {
		field = (DataObjectField *)mapIterator_item(&fiter, (void **)&key);
		syncOp = protocol_serializeField(dobj, key);
		asn_sequence_add(&p->choice.blockSyncListP.list, syncOp);
	}

	startIdx = *index;
	loop = 0;
	for (dataobject_childIterator(dobj, &iter); !listIterator_finished(&iter);
			listIterator_next(&iter)) {
		child = (DataObject *)listIterator_item(&iter);
		if (loop != 0) {
			syncOp = protocol_goToTree(startIdx);
			asn_sequence_add(&p->choice.blockSyncListP.list, syncOp);
		}
		++(*index);
		syncOp = protocol_addChild();
		asn_sequence_add(&p->choice.blockSyncListP.list, syncOp);
		cache_writeObjectR(p, child, rcFile, index, modifiedOnly);
		++loop;
	}

	return 0;
}

DataObject *cache_readObject(File *rcFile)
{
	int index;
	SyncListP_t *p;
	int fsize, res, bpos;
	void *buffer;
	asn_codec_ctx_t ctx;
	asn_dec_rval_t retval;
	DataObject *output;

#ifdef CACHE_DISABLE
	return NULL;
#endif

	fsize = file_size(rcFile);
	buffer = p_malloc(fsize);
	res = file_read(rcFile, fsize, buffer);
	if (res != fsize) {
		emo_printf("Failed to read cache, got %d bytes instead of %d" NL,
				res, fsize);
		p_free(buffer);
		return NULL;
	}
	bpos = 0;

	memset(&ctx, 0, sizeof(asn_codec_ctx_t));
	ctx.max_stack_size = 30000;
	p = NULL;
	retval = uper_decode_complete(&ctx, &asn_DEF_SyncListP,
			(void **)&p, buffer, fsize);
	p_free(buffer);
	switch (retval.code) {
		case RC_OK:
			break;
		case RC_WMORE:
			emo_printf("Cache truncated" NL);
			return NULL;
		case RC_FAIL:
			emo_printf("Cache decode failure" NL);
			return NULL;
	}

	index = 0;
	output = dataobject_new();
	cache_readObjectR(p, output, rcFile, &index);

	return output;
}

static int cache_readObjectR(SyncListP_t *sp, DataObject *root, File *rcFile, int *index)
{
	SyncOperandP_t *syncOp;
	DataObjectField *dof;
	DataObject *dobj, *cobj;
	char *fieldName;
	void *data;
	int i, treeIndex;
	EmoField fieldEnum = -1;
	struct blockSyncListP *p;

	dobj = root;
	p = &sp->choice.blockSyncListP;

	for (i = 0; i < p->list.count; ++i) {
		syncOp = p->list.array[i];
		// PROTOCOLFIX
		if (syncOp->fieldNameP.present == FieldNameP_PR_fieldNameEnumP)
			fieldEnum = syncOp->fieldNameP.choice.fieldNameEnumP;
		else
			fieldName = (char *)syncOp->fieldNameP.choice.fieldNameStringP.buf;
		switch (syncOp->syncP.present) {
			case syncP_PR_syncSetP:
				dof = dataobjectfield_string(
						(const char *)syncOp->syncP.choice.syncSetP.buf);
				if (fieldEnum == -1)
					dataobject_setValue(dobj, fieldName, dof);
				else
					dataobject_setEnum(dobj, fieldEnum, dof);
				break;
			case syncP_PR_syncModifyP:
				data = p_malloc(syncOp->syncP.choice.syncSetP.size);
				memcpy(data, syncOp->syncP.choice.syncSetP.buf,
						syncOp->syncP.choice.syncSetP.size);
				dof = dataobjectfield_data(data, syncOp->syncP.choice.syncSetP.size);
				if (fieldEnum == -1)
					dataobject_setValue(dobj, fieldName, dof);
				else
					dataobject_setEnum(dobj, fieldEnum, dof);
				break;
			case syncP_PR_nodeOperationP:
				if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeAddP) {
					if (syncOp->syncP.choice.nodeOperationP.choice.nodeAddP == nodeAddP_nodeChildP) {
						cobj = dataobject_new();
						dataobject_pack(dobj, cobj);
						dobj = cobj;
					} else {
						emo_printf("Cache Unsuported node add type" NL);
					}
				} else if (syncOp->syncP.choice.nodeOperationP.present == nodeOperationP_PR_nodeGotoTreeP) {
					treeIndex = syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP;
					dobj = dataobject_getTree(root, treeIndex);
					if (dobj == NULL) {
						emo_printf("Cache Going to invalid index" NL);
						emo_abort;
						return 1;
					}
				}
				break;
		}
	}

	return 0;
}


int cache_commitServerSide(DataObject *dobj, URL *url)
{
	File *rcFile;
	int serverIndex, resourceIndex, res;
	char filename[256], oldfilename[256];
	
#ifdef CACHE_DISABLE
	return 0;
#endif

	EMO_ASSERT_INT(cacheManager != NULL, 1, "commiting to cache without manager")
	EMO_ASSERT_INT(dobj != NULL, 1, "commiting NULL object to cache")
	EMO_ASSERT_INT(url != NULL, 1, "commiting object to cache missing url")

	cache_commitIndexs(url, &serverIndex, &resourceIndex, 1);

	snprintf(filename, 256, "cache/%d", serverIndex);
	file_mkdir(filename);
	snprintf(filename, 256, "cache/%d/%d.n", serverIndex, resourceIndex);
	snprintf(oldfilename, 256, "cache/%d/%d", serverIndex, resourceIndex);
	rcFile = file_openWrite(filename);
	if (rcFile == NULL) {
		emo_printf("Failed to write server side object cache" NL);
		return 1;
	}
	res = cache_writeObject(dobj, rcFile, 0);
	file_close(rcFile);
	if (res) {
		emo_printf("Cache failed to commit server side" NL);
		return 1;
	}
	return file_move(filename, oldfilename);
}

int cache_commitClientSide(DataObject *dobj, URL *url)
{
	return 0;
}

DataObject *cache_loadObject(URL *url)
{
	File *rcFile;
	int serverIndex, resourceIndex;
	char filename[256], oldfilename[256];
	DataObject *output;
	
#ifdef CACHE_DISABLE
	return NULL;
#endif

	EMO_ASSERT_NULL(cacheManager != NULL, "commiting to cache without manager")

	if (cache_commitIndexs(url, &serverIndex, &resourceIndex ,0))
		return NULL;

	snprintf(filename, 256, "cache/%d/%d", serverIndex, resourceIndex);
	snprintf(oldfilename, 256, "cache/%d/%dc", serverIndex, resourceIndex);
	rcFile = file_openRead(filename);
	if (rcFile == NULL) {
		return NULL;
	}
	output = cache_readObject(rcFile);
	file_close(rcFile);
	if (output == NULL) {
		emo_printf("Cache failed to load existing object" NL);
		return NULL;
	}
	return output;
}

static DataObject *load_manager(void)
{
	File *mgrFile;
	DataObject *output;

	mgrFile = file_openRead("cache/meta");
	if (mgrFile != NULL) {
		output = cache_readObject(mgrFile);
		file_close(mgrFile);
		if (output != NULL)
			return output;
	}

	emo_printf("Cache metadata not found, recreating" NL);
	output = dataobject_new();
	dataobject_setEnum(output, EMO_FIELD_TYPE, dataobjectfield_string("CacheManager"));
	return output;
}

static int save_manager(DataObject *mgr)
{
	File *mgrFile;
	int result;

	mgrFile = file_openWrite("cache/meta.n");
	if (mgrFile == NULL) {
		emo_printf("Failed to open cache metadata output" NL);
		return 1;
	}
	result = cache_writeObject(mgr, mgrFile, 0);
	file_close(mgrFile);
	if (result) {
		emo_printf("Failed to write cache metadata" NL);
		return 1;
	}
	result = file_move("cache/meta.n", "cache/meta");
	return result;
}

int cache_commit(void)
{
#ifdef CACHE_DISABLE
	return 0;
#endif
	return save_manager(cacheManager);
}