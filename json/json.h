#pragma once

#include "../stream/stream.h"
#ifdef _DEBUG
#include <assert.h>
#endif

#define JSON_OBJ			1 //AKA. MAP
#define JSON_ARRAY			4 //ARRAY OF JSONOBJ/TEXT/NUMBER/ARRAY/RAWDATA 
#define JSON_TEXT			2 //
#define JSON_STRING			2 // SAME AS TEXT
#define JSON_NUMBER			3 //
#define JSON_RAWDATA		5 //array of raw data

#define JSON_ERROR_OUTOFMEMORY -8
#define JSON_ERROR_INVALIDDATA -4

class _jsonobj;
struct jsonobj_functable {
	int (*Type)();
	long (*Create)(_jsonobj **,void *(*alloc)(unsigned long));
	void (*Delete)(_jsonobj*, void (*free)(void*));
	long (*Load)(_jsonobj*, stream *, char, void* (*alloc)(unsigned long), void (*free)(void*));
};
extern jsonobj_functable jsonobj_ftable;
extern jsonobj_functable jsonarray_ftable;
extern jsonobj_functable jsonraw_ftable;
extern jsonobj_functable jsonstring_ftable;
extern jsonobj_functable jsonnumber_ftable;

class _jsonobj {
public:
	jsonobj_functable* m_ftable;
	//other variables go here
};

class jsonkeypair {
public:
	char* key;
	_jsonobj* val;
	jsonkeypair* next;
	long Init() {
		this->key = 0;
		this->val = 0;
		this->next = 0;
		return 0;
	}
	long Init(void *(*alloc)(unsigned long),const char* key, _jsonobj* val = 0) {
		if (key) {
			unsigned long i = 0;
			while (key[i]) i++;
			this->key = (char*)alloc(sizeof(char)*(i + 1));
			if (!this->key) return JSON_ERROR_OUTOFMEMORY;
			while (key[i]) { this->key[i] = key[i++]; }
			this->key[i] = 0; //null terminate
		}
		this->val = val;
		this->next = 0;
		return 0;
	}
	long Init(void *(*alloc)(unsigned long),const char* key, unsigned long kLen, _jsonobj* val = 0) {
		if (key && kLen) {
			unsigned long i = 0;
			this->key = (char*)alloc(sizeof(char)*(kLen + 1));
			if (!this->key) return JSON_ERROR_OUTOFMEMORY;
			while (i < kLen) { this->key[i] = key[i++]; }
			this->key[i] = 0; //null terminate
		}
		this->val = val;
		this->next = 0;
		return 0;
	}
	void Free(void (*free_func)(void*)) {
		if (key) free_func(key); 
		if (val) { 
			val->m_ftable->Delete(val, free_func);
		} 
		next = 0; val = 0; key = 0;
	}
};

class jsonobj : public _jsonobj {
public:
	unsigned long m_keycount;
	unsigned long m_tablesize;
	jsonkeypair** m_table;
	jsonkeypair *operator [] (const char *p_key) {
		return Find(p_key);
	}
	jsonkeypair *operator [] (const unsigned long p_idx); //will convert integer to string first

	int ResizeTable(unsigned long size, void (*free)(void *), void* (*alloc)(unsigned long)) {
		if (size == 0) { 
			FreeTable(free);
			return 0; 
		} //will free key pairs too
		unsigned long size2 = 1;
		while (size2<size) {
			size2 <<= 1;
		}
		jsonkeypair** old_data = m_table;
		m_table = (jsonkeypair**)alloc(sizeof(jsonkeypair*)*size);
		if (!m_table) {
			m_table = old_data;
			return -1;
		}
		unsigned long old_size = m_tablesize;
		m_tablesize = size;
		if (old_data) {
			unsigned long i;
			for (i = 0; i < old_size; i++) {
				jsonkeypair* itr = old_data[i];
				while (itr) {
					jsonkeypair* next = itr->next;
					unsigned long k = calculateHashKey(itr->key);
					itr->next = m_table[k]; m_table[k] = itr; //insert at beginning for speed
					itr = next;
				}
			}
			free(old_data);
		}
		return 0;
	}

	unsigned long calculateHashKey(const char *p_key) {
		const unsigned long mask = m_tablesize - 1;
		unsigned long key = _keyHash(p_key) & mask;
		return key;
	}
	static unsigned long _keyHash(const char*p_key) {
		unsigned long hash = 0, i = 0;
		while (p_key[i] && i < 256/*bytes*/) {
			if (!(i & 3)) {
				hash ^= (i & 7) ? 0x55555555 : 0xaaaaaaaa;
			}
			hash += ((unsigned long)p_key[i]) << (i & 3);
			i++;
		}
		hash ^= i;
		return hash;
	}

	void FreeTable(void (*free)(void *)) {
		if (m_table) {
			unsigned long i;
			for (i = 0; i < m_tablesize; i++) {
				jsonkeypair* first = m_table[i];
				while(first) {
					jsonkeypair* next = first->next;
					first->Free(free); //will call Delete() on val
					free(first);
					first = next;
				}
			}
			free(m_table);
		}
	}
	//Load from within another object
	long Load(stream* buf, void* (*alloc)(unsigned long), void (*free)(void*));

	long Send(stream *buf, int pretty=0);

	jsonkeypair *Find(const char *key);
	//unsigned long NumKeys(jsonkeypair *p_firstChild) {
	//	unsigned count = 0;
	//	jsonkeypair* next = p_firstChild;
	//	while(next){
	//		count++;
	//		next = next->next;
	//	}
	//	return count;
	//}
	//unsigned long NumKeys() {
	//	unsigned long i;
	//	for (i = 0; i < m_size; i++) {
	//		count += NumKeys(m_table[i]);
	//	}
	//	return count;
	//}
	unsigned long numKeys() { return m_keycount;  }

	long addChild(jsonkeypair* child, void (*free)(void*),void*(*alloc)(unsigned long)) {
		unsigned long decentTableSize = ((m_keycount+4/*include the one to add*/) / 4); //for faster lookup we can / 1
		if (decentTableSize>m_tablesize) {
			int err = ResizeTable(decentTableSize,free,alloc);
			if (err <= 0) return JSON_ERROR_OUTOFMEMORY;
		}
		unsigned long key = calculateHashKey(child->key);
		child->next = m_table[key];
		m_table[key] = child;
		return 0;
	}

	jsonkeypair *AppendText(const char *key, const char *val, void*(*alloc)(unsigned long), void(*free)(void*));
	jsonkeypair *AppendText(const char *key, const char *val, const unsigned long len, void*(*alloc)(unsigned long), void(*free)(void*));
	jsonkeypair *AppendObj(const char *key, _jsonobj* val, void*(*alloc)(unsigned long), void(*free)(void*));
	jsonkeypair *AppendNumber(const char *key, double num, void*(*alloc)(unsigned long), void(*free)(void*));
};
int jsonobj_Type();
void jsonobj_Delete(_jsonobj*, void (*)(void*));
long jsonobj_Create(_jsonobj**, void *(*)(unsigned long));
long jsonobj_Load(_jsonobj*, stream*,char,void* (*)(unsigned long),void(*)(void*));

class jsonarray : public _jsonobj {
public:
	_jsonobj** m_data;
	unsigned long m_size;
	unsigned long m_totalsize;
	_jsonobj* operator [] (const unsigned long p_idx) {
		return m_data[p_idx];
	}

	long Resize(unsigned long p_newsize, void (*free)(void*),void *(*alloc)(unsigned long)) {
		//assume new never fails
		//size^2 >= p_newsize
		_jsonobj** nArray = p_newsize ? ((_jsonobj**)alloc(sizeof(_jsonobj*)*p_newsize)) : 0;
		if (!nArray && p_newsize) return JSON_ERROR_OUTOFMEMORY;
		if (m_data) {
			unsigned long i;
			unsigned long smallerSize = p_newsize;
			if (m_size < p_newsize) smallerSize = m_size;
			for (i = 0; i < smallerSize; i++) {
				nArray[i] = m_data[i]; //copy pointers
			}
			for (; i < m_size; i++) {
				//if size is smaller then new size free all objects
				m_data[i]->m_ftable->Delete(m_data[i],free);
			}
			free(m_data);
			m_size = smallerSize;
		}
		else {
			m_size = 0;
		}
		m_totalsize = p_newsize;
		m_data = nArray;
		return 0;
	}

	void Free(void (*free)(void*)) {
		Resize(0, free, 0/*alloc will not be used*/);
	}

	long Load(stream* buf, void* (*alloc)(unsigned long), void (*free)(void*));	

	long AppendObj(_jsonobj* val, void* (*alloc)(unsigned long), void (*free)(void*)) {
		if (m_size >= m_totalsize) {
			int err = Resize(m_totalsize ? m_totalsize * 2 : 4, free, alloc);
			if (err < 0) return err;
		}
		m_data[m_size] = val;
		m_size++;
		return 0;
	}
};
int jsonarray_Type();
void jsonarray_Delete(_jsonobj*, void (*)(void*));
long jsonarray_Create(_jsonobj**, void* (*)(unsigned long));
long jsonarray_Load(_jsonobj*, stream*, char, void* (*)(unsigned long), void(*)(void*));

class jsonstring : public _jsonobj {
public:
	char *m_str;
	//make sure m_str is not allocated before calling this
	long SetString(const char *s, void *(*alloc)(unsigned long)) {
		unsigned long i = 0;
		if (s) {
			for (;s[i]; i++) {}
			m_str = (char*)alloc(sizeof(char)*(i + 1));
			if(!m_str) return JSON_ERROR_OUTOFMEMORY;
			for (i=0; s[i]; i++) {
				m_str[i] = s[i];
			}
			m_str[i] = 0;
		}
		return i;
	}
	long SetString(const char *s, unsigned long len, void *(*alloc)(unsigned long)) {
		unsigned long i = 0;
		if (s) {
			m_str = (char*)alloc(sizeof(char)*(len + 1));
			if(!m_str) return JSON_ERROR_OUTOFMEMORY;
			for (i=0; i<len; i++) {
				m_str[i] = s[i];
			}
			m_str[i] = 0;
		}
		return i;
	}
	char &operator[](long i) {
		return m_str[i];
	}

};
int jsonstring_Type();
void jsonstring_Delete(_jsonobj*, void (*)(void*));
long jsonstring_Create(_jsonobj**, void* (*)(unsigned long));
long jsonstring_Load(_jsonobj*, stream*, char, void* (*)(unsigned long), void(*)(void*));

class jsonnumber : public _jsonobj {
public:
	double num;
};
int jsonnumber_Type();
void jsonnumber_Delete(_jsonobj*, void (*)(void*));
long jsonnumber_Create(_jsonobj**, void* (*)(unsigned long));
long jsonnumber_Load(_jsonobj*, stream*, char, void* (*)(unsigned long), void(*)(void*));

	int JSON_movepastwhite(stream *buf);

	//int JSON_sendObj(stream *buf, struct t_jsonobj *obj);
	long JSON_send(stream *buf, _jsonobj *p_obj, int pretty);

	int JSON_parseVal(_jsonobj** obj, char lastch, stream* buf, void* (*alloc)(unsigned long), void (*free)(void*));

	int JSON_parseString_iterateNumber(char *result, stream *buf) ;
	int JSON_parseString_iterateQuote(char* result, char quote, stream* buf);
	int JSON_parseString_iterateQuote_getLength(char quote, stream* buf);


