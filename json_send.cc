#define _CRT_SECURE_NO_WARNINGS

#include "json.h"
#include <string.h>
#include <stdio.h>

long jsonobj::Send(stream *buf, int pretty) {
	return JSON_send(buf, this, pretty);
}
jsonobj *jsonobj::FindByKey(const char *key) {
	return JSON_findKey(this, key);
}

int JSON_sendtext(stream *buf, char *text) {
	int i;
	int len = (int)strlen(text);
	char miniBuf[32];
	miniBuf[0] = '\"';
	int bufSize = 1;
	for (i = 0; i < len; i++) {
		if (text[i] == '\\' || text[i] == '\n' || text[i] == '\r' || text[i] == '\"' || text[i] == '\'' ) {
			miniBuf[bufSize++] = '\\'; miniBuf[bufSize++] = text[i];
		}
		else {
			miniBuf[bufSize++] = text[i];
		}
		if (bufSize > 30) { //then flush miniBuf
			int iResult = buf->PutBytes(miniBuf, bufSize);
			bufSize = 0; 
			if (iResult < 0) return iResult;
		}
	}
	miniBuf[bufSize++] = '\"';
	return buf->PutBytes( miniBuf, bufSize); //send rest of it
}

int JSON_sendchildren(stream *buf, jsonobj *children, int pretty);
int JSON_send(stream *buf, jsonobj *obj, int pretty) {
	int iResult;
	if (obj->name) { //null if array item
		iResult = JSON_sendtext(buf, obj->name);
		if (iResult < 0) return iResult;
		iResult = buf->PutBytes(":", 1);
		if (iResult < 0) return iResult;
	}
	char tmp[32];
	switch (obj->type) {
	case JSON_NUMBER:
		iResult = sprintf(tmp, "%f", obj->valnum);
		return buf->PutBytes(tmp, iResult);
	case JSON_TEXT:
		return JSON_sendtext(buf, obj->val);
	case JSON_OBJ:
		iResult = buf->PutBytes( "{", 1);
		if (pretty == 1 && iResult > 0)
			iResult = buf->PutBytes("\n  ", 3);
		if (iResult < 0) return iResult;
		iResult = JSON_sendchildren(buf, obj->valobj,pretty);
		if (pretty == 1 && iResult >= 0)
			iResult = buf->PutBytes("\n", 1);
		if (iResult < 0) return iResult;
		iResult = buf->PutBytes( "}", 1);
		return iResult;
	case JSON_ARRAY:
		iResult = buf->PutBytes( "[", 1);
		if (pretty == 1 && iResult > 0)
			iResult = buf->PutBytes("\n  ", 3);
		if (iResult < 0) return iResult;
		iResult = JSON_sendchildren(buf, obj->valobj,pretty);
		if (pretty == 1 && iResult >= 0)
			iResult = buf->PutBytes("\n", 1);
		if (iResult < 0) return iResult;
		iResult = buf->PutBytes("]", 1);
		return iResult;
	default:
		return buf->PutBytes( "{}",2);
	}
}

int JSON_sendchildren(stream *buf, jsonobj *children, int pretty) {
	jsonobj *itr = children;
	int iResult =0;
	while (itr) {
		iResult = JSON_send(buf, itr, pretty);
		if (iResult < 0) return iResult;

		if (itr->next) {
			iResult = buf->PutBytes( ",", 1);
			if (pretty == 1 && iResult > 0)
				iResult = buf->PutBytes("\n  ", 3);
			if (iResult < 0) return iResult;
		}
		itr = itr->next;
	}
	return iResult;
}

