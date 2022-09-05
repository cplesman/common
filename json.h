#pragma once

#include "stream/stream.h"

#define JSON_OBJ			1 
#define JSON_ARRAY			4 
#define JSON_TEXT			2 //
#define JSON_NUMBER			3 //

#define JSON_ERROR_OUTOFMEMORY -8
#define JSON_ERROR_INVALIDDATA -4
	//TODO: create a hash table inside json obj if its too large
	class jsonobj{
	public:
		char *name; //if name is null then obj is item in an array or top parent
		union {
			char *val/*(s)*/; //can be json obj, array, text ( linked list of jsonobj, or null terminated text )
			jsonobj *valobj;
			double valnum;
		};
		long type;
		jsonobj *next;

		jsonobj() {
			name = 0; type = JSON_OBJ;
			next = 0;
		}
		~jsonobj() {
			if (name) delete[] name;
			if (type == JSON_OBJ || type == JSON_ARRAY) {
				FreeChildren();
			}
			else if (type != JSON_NUMBER && val) {
				delete[] val;
			}
		}

		void FreeChildren() {
			jsonobj *itr=valobj/*children*/;
			while (itr) {
				jsonobj *next = itr->next;
				delete itr;
				itr = next;
			}
		}

		long Load(stream *buf);
		long Send(stream *buf, int pretty=0);
		jsonobj *FindByKey(const char *key);
		jsonobj *AppendText(const char *key, const char *val);
		jsonobj *AppendText(const char *key, const int klen, const char *val, const int len);
		jsonobj *AppendHex(const char *key, const char *data, long dataSize);
		jsonobj *AppendObj(const char *key, int type);
		jsonobj *AppendNumber(const char *key, double num);
	};

	jsonobj *JSON_allocate();
	jsonobj *JSON_allocateTextObj(long keySize, long valSize);
	void JSON_free(jsonobj *obj);
	int JSON_parseData(jsonobj **result, stream *buf);

	long JSON_countChildren(jsonobj*parent);

	int JSON_movepastwhite(stream *buf);

	jsonobj **JSON_getIndexPtr(jsonobj *parent, const int i);
	jsonobj **JSON_findKeyPtr(jsonobj *parent, const char *key);
	jsonobj *JSON_getIndex(jsonobj *parent, const int i);
	jsonobj *JSON_findKey(jsonobj *parent, const char *key);

	jsonobj *JSON_createText(const char *key, const char *val);
	jsonobj *JSON_createText(const char *key, const int klen, const char *val, const int len);
	jsonobj *JSON_createHex(const char *key, const char *data, long dataSize);
	jsonobj *JSON_createObj(const char *key, int type);

	int JSON_prependObj(jsonobj *parent, jsonobj *child);
	
	void JSON_removeObj(jsonobj *parent, const char *key);

	//int JSON_sendObj(stream *buf, struct t_jsonobj *obj);
	int JSON_send(stream *buf, jsonobj *obj, int pretty);

