#define _CRT_SECURE_NO_WARNINGS

#include "json.h"

#include "hexdata.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

jsonobj *JSON_createText(const char *key, const char *val) {
	jsonobj *child = JSON_allocateTextObj((long)strlen(key), (long)strlen(val));
	if (!child) return 0;
	strcpy(child->name, key);
	strcpy(child->val, val);
	return child;
}
jsonobj *JSON_createText(const char *key, const int klen, const char *val,const int len) {
	jsonobj *child = JSON_allocateTextObj((long)klen, (long)len);
	if (!child) return 0;
	strncpy(child->name, key, (size_t)klen);
	child->name[klen] = 0;
	strncpy(child->val, val, (size_t)len);
	child->val[len] = 0;
	return child;
}

jsonobj *JSON_createHex(const char *key, const char *data, long dataSize) {
	jsonobj *child = JSON_allocateTextObj((long)strlen(key), dataSize * 2);
	if (!child) return 0;
	strcpy(child->name, key);
	binToHexString(child->val, (unsigned char*)data, dataSize);
	return child;
}

jsonobj *JSON_createObj(const char *key, int type) {
	jsonobj *child = JSON_allocate();
	if (!child)
		return 0;
	child->type = type;
	if (key) {
		int alloclen = (int)strlen(key) + 1;
		child->name/*key*/ = new char[alloclen];
		if (!child->name) {
			JSON_free(child);
			return 0;
		}
		memcpy(child->name, key, alloclen);
	}
	return child;
}


jsonobj *jsonobj::AppendText(const char *key, const char *val) {
	jsonobj *obj;
	JSON_prependObj(this, obj = JSON_createText(key, val));
	return obj;
}
jsonobj *jsonobj::AppendText(const char *key, const int klen, const char *val,const int len) {
	jsonobj *obj;
	JSON_prependObj(this, obj = JSON_createText(key,klen, val,len));
	return obj;
}
jsonobj *jsonobj::AppendHex(const char *key, const char *data, long dataSize) {
	jsonobj *obj;
	JSON_prependObj(this, obj = JSON_createHex(key, data,dataSize));
	return obj;
}
jsonobj *jsonobj::AppendObj(const char *key, int type) {
	jsonobj *obj;
	JSON_prependObj(this, obj = JSON_createObj(key, type));
	return obj;
}
jsonobj *jsonobj::AppendNumber(const char *key, double num) {
	jsonobj *obj;
	JSON_prependObj(this, obj = JSON_createObj(key, JSON_NUMBER));
	obj->valnum = num;
	return obj;
}
