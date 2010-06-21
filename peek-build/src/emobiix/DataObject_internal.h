#ifndef _DATAOBJECT_INTERNAL_H_
#define _DATAOBJECT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

struct DataObject_t;
typedef struct DataObject_t DataObject;
typedef enum {DOF_STRING, DOF_DATA, DOF_INT, DOF_UINT} DOFType;

struct DataObjectField_t {
	DOFType type;
	union field_t {
		char *string;
		struct data_t {
			unsigned char *bytes;
			int size;
		} data;
		int integer;
		unsigned int uinteger;
	} field;
};
typedef struct DataObjectField_t DataObjectField;

#ifdef __cplusplus
}
#endif

#endif

