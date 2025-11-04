#include "json.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int jsonobj_Type() { return JSON_OBJ; }
int jsonarray_Type() { return JSON_ARRAY; }
int jsonstring_Type() { return JSON_STRING; }
int jsonnumber_Type() { return JSON_NUMBER; }
int jsonboolean_Type() { return JSON_BOOLEAN; }

long jsonobj_Create(_jsonobj** p_obj, void* (*alloc)(unsigned long)) {
	jsonobj *obj = (jsonobj*)alloc(sizeof(jsonobj));
	if (!obj) return JSON_ERROR_OUTOFMEMORY;
	*p_obj = obj;
	obj->m_ftable = &jsonobj_ftable;
	obj->m_tablesize = 0;
	obj->m_table = 0;
	return 0;
}
long jsonarray_Create(_jsonobj** p_obj, void* (*alloc)(unsigned long)) {
	jsonarray* obj = (jsonarray*)alloc(sizeof(jsonarray));
	if (!obj) return JSON_ERROR_OUTOFMEMORY;
	*p_obj = obj;
	obj->m_data = 0;
	obj->m_size = 0;
	obj->m_totalsize = 0;
	obj->m_ftable = &jsonarray_ftable;
	return 0;
}
long jsonstring_Create(_jsonobj** p_obj, void* (*alloc)(unsigned long)) {
	jsonstring* obj = (jsonstring*)alloc(sizeof(jsonstring));
	if (!obj) return JSON_ERROR_OUTOFMEMORY;
	*p_obj = obj;
	obj->m_str = 0;
	obj->m_ftable = &jsonstring_ftable;
	return 0;
}
long jsonnumber_Create(_jsonobj** p_obj, void* (*alloc)(unsigned long)) {
	jsonnumber* obj = (jsonnumber*)alloc(sizeof(jsonnumber));
	if (!obj) return JSON_ERROR_OUTOFMEMORY;
	*p_obj = obj;
	obj->num = 0;
	obj->m_ftable = &jsonnumber_ftable;
	return 0;
}
long jsonboolean_Create(_jsonobj** p_obj, void* (*alloc)(unsigned long)) {
	jsonboolean* obj = (jsonboolean*)alloc(sizeof(jsonboolean));
	if (!obj) return JSON_ERROR_OUTOFMEMORY;
	*p_obj = obj;
	obj->b = false;
	obj->m_ftable = &jsonboolean_ftable;
	return 0;
}

void jsonobj_Delete(_jsonobj* p_obj, void (*free)(void*)) {
	jsonobj* obj = (jsonobj*)p_obj;
	obj->FreeTable(free);
	free(obj);
}
void jsonarray_Delete(_jsonobj* p_obj, void (*free)(void*)) {
	jsonarray* obj = (jsonarray*)p_obj;
	if(obj->m_size){
		obj->Resize(0, free, 0/*alloc will not be used*/);
	}
	if (obj->m_data) {
		//m_size may have been 0 but totalsize >0
		free(obj->m_data);
	}
	free(obj);
}
void jsonstring_Delete(_jsonobj* p_obj, void (*free)(void*)) {
	jsonstring* obj = (jsonstring*)p_obj;
	if(obj->m_str){
		free(obj->m_str);
	}
	free(obj);
}
void jsonnumber_Delete(_jsonobj* p_obj, void (*free)(void*)) {
	jsonnumber* obj = (jsonnumber*)p_obj;
	free(obj);
}
void jsonboolean_Delete(_jsonobj* p_obj, void (*free)(void*)) {
	jsonboolean* obj = (jsonboolean*)p_obj;
	free(obj);
}

jsonobj_functable jsonobj_ftable = {
	jsonobj_Type,
	jsonobj_Create,
	jsonobj_Delete,
	jsonobj_Load,
};
jsonobj_functable jsonarray_ftable = {
	jsonarray_Type,
	jsonarray_Create,
	jsonarray_Delete,
	jsonarray_Load
};
jsonobj_functable jsonstring_ftable = {
	jsonstring_Type,
	jsonstring_Create,
	jsonstring_Delete,
	jsonstring_Load
};
jsonobj_functable jsonnumber_ftable = {
	jsonnumber_Type,
	jsonnumber_Create,
	jsonnumber_Delete,
	jsonnumber_Load
};
jsonobj_functable jsonboolean_ftable = {
	jsonboolean_Type,
	jsonboolean_Create,
	jsonboolean_Delete,
	jsonboolean_Load
};

_jsonobj *jsonobj::operator [] (const unsigned long p_idx) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%lu", p_idx);
	//ultoa(p_idx, buf, 10);
	return Find(buf);
}


//jsonobj *JSON_findChild(darray<jsonobj *> *p_children, const char *p_key) {
//	if (!p_children) {
//		return nullptr;
//	}
//	unsigned long size = p_children->Size();
//	for(unsigned long i=0;i<size;i++) {
//		jsonobj*itr = p_children->At(i);
//		if (!strcmp(p_key, itr->name)) {
//			return itr;
//		}
//	}
//	return 0;
//}
_jsonobj *jsonobj::Find(const char *p_key) {
	unsigned long k = calculateHashKey(p_key);
	jsonkeypair* itr = m_table[k];
	while (itr) {
		if (!strcmp(p_key, itr->key)) {
			return itr->val;
		}
		itr = itr->next;
	}
	return 0; //nothing found
}

int JSON_movepastwhite(stream *buf) {
	char ch;
	do {
		int err = buf->GetBytes(&ch, 1);
		if (err < 0) return err;
	} while ((ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t'));
	return ch;
}

//void JSON_addChildren(darray<darray<jsonobj*>*> *p_table, darray<jsonobj*> *p_children) {
//	unsigned long tsize = p_table->Size();
//#ifdef _DEBUG
//	unsigned long validtablesize = tsize;
//	while (validtablesize && !(validtablesize & 1)) {
//		validtablesize >>= 1;
//	}
//	assert(validtablesize == 1);
//#endif 
//	const unsigned long mask = tsize - 1;
//	unsigned long i;
//	const unsigned long numChildren = p_children->Size();
//	for(i=0;i<numChildren;i++) {
//		jsonobj *child = p_children->At(i);
//		unsigned long key = (jsonobj::getKeyHash(child->name) & mask);
//		darray<jsonobj*> *tchildren = p_table->At(key);
//		if (!tchildren) {
//			tchildren = (p_table->At(key) = new darray<jsonobj*>(1));
//		}
//		tchildren->push(child);
//	}
//}

int JSON_parseVal(_jsonobj **p_val, char lastch, stream *buf, void *(*alloc)(unsigned long), void (*free)(void*)) {
	int err;
	char ch = lastch;
	//determine type
	if (ch == '{') { //obj in obj
		err = jsonobj_Create(p_val, alloc);
		if (err < 0) { return err; }

		err = jsonobj_Load(*p_val,buf, ch, alloc, free);
		if (err < 0) { jsonobj_Delete(*p_val, free); *p_val = 0; return err; }

		return '}'; //ch
	}
	else if (ch == '[') {
		err = jsonarray_Create(p_val, alloc);
		if (err < 0) { return err; }

		err = jsonarray_Load(*p_val,buf, ch, alloc, free);
		if (err < 0) { if(*p_val) jsonarray_Delete(*p_val, free); *p_val = 0; return err; }

		return ']';//ch
	}
	else if (ch == '\'' || ch == '\"') {
		err = jsonstring_Create(p_val, alloc);
		if (err < 0) { if(*p_val) return err; }

		err = jsonstring_Load(*p_val,buf, ch, alloc, free);
		if (err < 0) { jsonstring_Delete((jsonobj*)&p_val, free); *p_val = 0; return err; }

		return err; 
	}
	else if ((ch >= '0'&& ch <= '9')||ch=='-') {
		err = jsonnumber_Create(p_val, alloc);
		if (err < 0) { if(*p_val) return err; }

		err = jsonnumber_Load(*p_val,buf, ch, alloc, free);
		if (err < 0) { jsonnumber_Delete((jsonobj*)&p_val, free); *p_val = 0; return err; }

		return err; 
	}
	else if( (ch=='t') || (ch=='f') ) {
		err = jsonboolean_Create(p_val, alloc);
		if (err < 0) { if(*p_val) return err; }

		err = jsonboolean_Load(*p_val,buf, ch, alloc, free);
		if (err < 0) { jsonboolean_Delete((jsonobj*)&p_val, free); *p_val = 0; return err; }

		return err; 
	}

	return -4; //not a valid obj
}

int JSON_parseString_iterateNumber(char *result, stream *buf) {
	int size = 0;
	char ch = 0;
	while (1) {
		int err;
		err = buf->PeekBytes(0, &ch, 1);
		if (err < 0) return err;
		if ((ch>='0'&&ch<='9') || ch=='.' || ch=='e' || ch=='-'){
			err = buf->GetBytes( &ch, 1);
			if (err < 0) return err;
		}
		else {
			break;
		}
		result[size] = ch;
		size++;
		if (size >= 30) return JSON_ERROR_INVALIDDATA;
	}
	*(result + size) = 0; //null terminate
	return size;
}
int JSON_parseString_iterateQuote_getLength(char quote, stream* buf) {
	int size = 0;
	char ch;
	while (1) {
		int err;
		err = buf->PeekBytes(size, &ch, 1);
		if (err < 0) return err;
		char ch2 = ch;
		if (ch == '\\') { //escape
			int err;
			err = buf->PeekBytes(size, &ch2, 1);
			if (err < 0) return err;
			if (ch2 == 'r') ch2 = '\r';
			else if (ch2 == 'n')ch2 = '\n';
		}
		if (ch == quote) {
			break;
		}
		size++;
	}
	size++;
	return size;
}
int JSON_parseString_iterateQuote(char* result, char quote, stream* buf) {
	int size = 0;
	char ch = 0;
	if (!result) return 0;
	while (1) {
		int err = buf->GetBytes(&ch, 1);
		if (err < 0) return err;
		char ch2 = ch;
		if (ch == '\\') { //escape
			int err;
			err = buf->GetBytes(&ch2, 1);
			if (err < 0) return err;
			if (ch2 == 'r') ch2 = '\r';
			else if (ch2 == 'n')ch2 = '\n';
		}
		if (ch == quote) {
			break;
		}
		*(result + size) = ch2;
		size++;
	}
	*(result + size) = 0; //null terminate
	size++;
	return size;
}
