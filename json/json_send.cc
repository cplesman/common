#define _CRT_SECURE_NO_WARNINGS

#include "json.h"
#include <string.h>
#include <stdio.h>

const char g_spacebytes[] = {
' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
0
};
int JSON_sendTab(stream *buf, int tab) {
	int t = tab;
	if (tab > 25) {
		t = 25;
	}
	return buf->PutBytes(g_spacebytes, t);
}

long jsonobj::Send(stream *buf, int pretty) {
	long iResult = 0;
	if (pretty) {
		iResult = JSON_sendTab(buf, pretty);
		if(iResult<0) return iResult;
	}
	iResult = JSON_send(buf, this, pretty);
	if(iResult<0) return iResult;

	return buf->PutBytes("\r\n", 2);
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

long JSON_sendchildren(stream *buf, _jsonobj *children[], unsigned long num_children, int pretty);
long JSON_send(stream *buf, _jsonobj* p_obj, int pretty);
long JSON_sendpair(stream *buf, jsonkeypair *pair, int pretty) {
	int cpretty = pretty ? pretty + 2 : 0;
	int iResult;
	if (pair->key) { //null if array item
		iResult = JSON_sendTab(buf, pretty);
		if (iResult < 0) return iResult;
		iResult = JSON_sendtext(buf, pair->key);
		if (iResult < 0) return iResult;
		iResult = buf->PutBytes(":", 1);
		if (iResult < 0) return iResult;
	}
	return JSON_send(buf, pair->val, cpretty);
}
long JSON_send(stream *buf, _jsonobj *p_obj, int pretty) {
	int cpretty = pretty ? pretty + 2 : 0;
	int iResult;
	char tmp[32];
	switch (p_obj->m_ftable->Type()) {
	case JSON_NUMBER:
		iResult = sprintf(tmp, "%f", ((jsonnumber*)p_obj)->num);
		return buf->PutBytes(tmp, iResult);
	case JSON_BOOLEAN:
		return buf->PutBytes( ((jsonboolean*)p_obj)->b ? "true" : "false", ((jsonboolean*)p_obj)->b ? 4 : 5);
	case JSON_TEXT:
		return JSON_sendtext(buf, ((jsonstring*)p_obj)->m_str);
	case JSON_OBJ:{
		jsonobj* obj = (jsonobj*)p_obj;
		unsigned long keycount = obj->NumKeys();
		iResult = buf->PutBytes( "{", 1);
		if (pretty && keycount && iResult > 0)
			iResult = buf->PutBytes("\r\n", 2);
		if (iResult < 0) return iResult;
		if (keycount) {
			unsigned long total = keycount,count=0;
			unsigned long tsize = obj->m_tablesize;
			for (unsigned long i = 0; i < tsize; i++) {
				jsonkeypair* itr = obj->m_table[i];
				while(itr){
					int err = JSON_sendpair(buf,itr, cpretty);
					if(err<0) return err;
					count++;
					if (count < total) {
						iResult = buf->PutBytes(",", 1);
					}
					if (pretty)
						iResult = buf->PutBytes("\r\n", 2);
					itr = itr->next;
				}
			}
			if (iResult < 0) return iResult;
			if (total&&pretty) {
				JSON_sendTab(buf, pretty);
			}
		}
		iResult = buf->PutBytes( "}", 1);
		return iResult;
	}
	case JSON_ARRAY:
		iResult = buf->PutBytes( "[", 1);
		if (pretty && iResult > 0)
			iResult = buf->PutBytes("\r\n", 2);
		if (iResult < 0) return iResult;
		iResult = JSON_sendchildren(buf, ((jsonarray*)p_obj)->m_data, ((jsonarray*)p_obj)->m_size, cpretty);
		if (iResult < 0) return iResult;
		if (pretty && iResult >= 0)
			iResult = buf->PutBytes("\r\n", 2);
		if (iResult>0&&pretty) {
			JSON_sendTab(buf, pretty);
		}
		iResult = buf->PutBytes("]", 1);
		return iResult;
	default:
		return buf->PutBytes( "{}",2);
	}
}

long JSON_sendchildren(stream *buf, _jsonobj *children[], unsigned long num_children, int pretty) {
	if (!children) {
		return 0; //no children
	}
	int iResult =0;
	for(unsigned long i=0;i<num_children;i++) {
		if(pretty) 
			JSON_sendTab(buf, pretty);
		iResult = JSON_send(buf, children[i], pretty);
		if (iResult < 0) return iResult;

		if (i+1<num_children) {
			//not last one
			iResult = buf->PutBytes( ",", 1);
			if (pretty )
				iResult = buf->PutBytes("\r\n", 2);
			if (iResult < 0) return iResult;
		}
	}
	return num_children;
}

