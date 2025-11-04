#include "json.h"
#include <stdlib.h>

using namespace std;

long jsonobj_Load(_jsonobj* p_obj, stream* buf, char lastCh, void* (*alloc)(unsigned long), void (*free)(void*)) {
	int err;
	char ch;
	jsonobj* obj = (jsonobj*)p_obj;

	//move to significant character
	err = JSON_movepastwhite(buf);
	if (err < 0) { return err; }
	ch = (char)err;
	if ((ch == '}' && lastCh == '{') || (!ch && !lastCh)) {
		return ch; //this is an empty object
	}

	jsonkeypair* keypair = 0;
	jsonkeypair** next = &keypair;
	unsigned long count = 0;

	//at this point the stream should have moved past '{'
	do {
		if (ch == '\'' || ch == '\"') {
			err = JSON_parseString_iterateQuote_getLength(ch, buf);
			if (err < 0) { break; }
			(*next) = (jsonkeypair*)alloc(sizeof(jsonkeypair));
			count++;
			if (!(*next)) { err = JSON_ERROR_OUTOFMEMORY; break; }
			(*next)->Init();
			(*next)->key = (char*)alloc(sizeof(char) * /*size with null*/err);
			if (!(*next)->key) { err = JSON_ERROR_OUTOFMEMORY; break; }
			err = JSON_parseString_iterateQuote((*next)->key, ch, buf);
			if (err < 0) { break; }
			//ch stays as quote
		}
		else {
			//expected key for the key pair
			err = JSON_ERROR_INVALIDDATA;
			break;
		}

		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		if (ch != ':') {
			err = JSON_ERROR_INVALIDDATA; break;
		}

		//move to next char after ':'
		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		err = JSON_parseVal(&(*next)->val, ch, buf, alloc,free);
		if (err < 0) { break; }
		ch = (char)err;

		//move to next char after ch
		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		if (ch == '}') break; //reached end of object
		if (ch != ',') { err = JSON_ERROR_INVALIDDATA; break; }

		//move to next char after ','
		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		next = &(*next)->next;

	} while (ch != '}'); //could be ',' then '}' which will be ok

	if(err>=0) {
		err = obj->ResizeTable(count,free,alloc);
	}

	jsonkeypair* itr = keypair;// firstpair
	if (err < 0) {
		while(keypair) {
			jsonkeypair* next = keypair->next;
			keypair->Free(free);
			free(keypair);
			keypair = next;
		}
		return err;
	}
	while (itr) {
		unsigned long key = obj->calculateHashKey(itr->key);
		jsonkeypair* next = itr->next; //save next because we are changing itr->next
		itr->next = obj->m_table[key];
		obj->m_table[key] = itr;

		itr = next;
	}

	return ch;
}

long jsonobj::Load(stream* buf, void* (*alloc)(unsigned long), void (*free)(void*)) {
	//move to '{'
	int err = JSON_movepastwhite(buf);
	if (err < 0) { return err; }
	char ch = (char)err;

	if (ch != '{') {
		return JSON_ERROR_INVALIDDATA; //syntax error
	}

	return jsonobj_Load(this, buf, '{', alloc,free);
}

long jsonarray::Load(stream* buf, void* (*alloc)(unsigned long), void (*free)(void*)) {
	//move to '{'
	int err = JSON_movepastwhite(buf);
	if (err < 0) { return err; }
	char ch = (char)err;

	if (ch != '[') {
		return JSON_ERROR_INVALIDDATA; //syntax error
	}

	return jsonarray_Load(this, buf, '[', alloc,free);
}

class jsonarrayitemlink {
public:
	void Init() { val = 0; next = 0; }
	_jsonobj* val;
	jsonarrayitemlink* next;
};
long jsonarray_Load(_jsonobj* p_obj, stream* buf, char lastCh, void* (*alloc)(unsigned long), void (*free)(void*)) {
	int err;
	char ch;
	jsonarray* obj = (jsonarray*)p_obj;
	jsonarrayitemlink* list = 0;
	jsonarrayitemlink** next = &list;
	unsigned long count = 0;

	//at this point the stream should have moved past '['
	do {
		//move to next char after ',' or '['
		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		if (ch == ']') break; //all done

		*next = (jsonarrayitemlink*)alloc(sizeof(jsonarrayitemlink));
		if (!(*next)) { err = JSON_ERROR_OUTOFMEMORY; break; }
		(*next)->Init();
		count++;
		err = JSON_parseVal(&(*next)->val, ch, buf,alloc,free); //on error parseVal will delete val
		if (err < 0) { break; }
		ch = (char)err;
		next = &(*next)->next;

		err = JSON_movepastwhite(buf);
		if (err < 0) { break; }
		ch = (char)err;

		if (ch == ']') break; //all done
		if (ch != ',') {
			err = JSON_ERROR_INVALIDDATA; break;
		}
	} while (1);

	if (err >= 0 && count) {
		err = obj->Resize(count,free,alloc);
		if(err>=0){
			jsonarrayitemlink* itr = list;
			count = 0;
			while (itr) {
				obj->m_data[count] = itr->val;
				count++;
				itr = itr->next;
			}
		}
		obj->m_size = count;
	}

	while(list){
		jsonarrayitemlink* next = list->next;
		free(list);
		list = next;
	}

	return err;
}

long jsonstring_Load(_jsonobj* p_obj, stream* buf, char lastCh, void* (*alloc)(unsigned long), void (*free)(void*)) {
	int err = JSON_parseString_iterateQuote_getLength(lastCh, buf);
	if (err < 0) { return err; }
	jsonstring* obj = (jsonstring*)p_obj;
	if(obj->m_str) free(obj->m_str);
	obj->m_str = (char*)alloc(sizeof(char) * /*size with null*/err);
	if (err < 0) { return err; }
	err = JSON_parseString_iterateQuote(obj->m_str, lastCh, buf);
	if (err < 0) { return err; }
	return lastCh;
}

long jsonnumber_Load(_jsonobj* p_obj, stream* buf, char lastCh, void* (*alloc)(unsigned long), void (*free)(void*)) {
	char numstr[32];
	numstr[0] = lastCh;
	int err = JSON_parseString_iterateNumber(numstr + 1, buf);
	if (err < 0) return err;
	((jsonnumber*)p_obj)->num = atof(numstr);
	return numstr[err]; //return last number as ch
}

long jsonboolean_Load(_jsonobj* p_obj, stream* buf, char lastCh, void* (*alloc)(unsigned long), void (*free)(void*)) {
	const char* boolstr = (lastCh == 't') ? "true" : "false";
	int i = 1;
	while(i< (boolstr[0]=='t'?4:5) ) {
		char ch;
		int err = buf->GetBytes(&ch, 1);
		if (err < 0) return err;
		if (ch != boolstr[i]) {
			return JSON_ERROR_INVALIDDATA;
		}
		i++;
	}
	((jsonboolean*)p_obj)->b = boolstr[0] == 't' ? true : false;
	return 'e'; //return last char read (always 'e' in true/false)
}
