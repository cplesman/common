#pragma once

#include "darray.h"

#ifdef _DEBUG
#include <assert.h>
#endif

class hashkey {
public:
	virtual~hashkey() {
		free();
	}

	virtual long compare(const hashkey *otherKey) = 0;
	virtual void free() {	}
	virtual unsigned long Key() const = 0; //for inserting and finding in tables
};

class hashstring :public hashkey{
public:
	char *m_key;
	hashstring(const char *key, long len) {
		m_key = 0;
		Init(key, len);
	}
	hashstring(const char *key) {
		m_key = 0;
		Init(key);
	}
	hashstring() {
		m_key = 0;
	}
	void Init(const char *key, long len) {
		if (m_key) {
			unsigned long prevLen = 0;
			char *itr2 = m_key;
			while (*itr2) { prevLen++; itr2++; };
			if (prevLen < len) {
				delete[] m_key;
				m_key = new char[len + 1];
			}
		}
		else {
			m_key = new char[len + 1];
		}
		char *itr = (char*)key; len = 0;
		while (*itr) { m_key[len] = *itr; len++; itr++; }
		m_key[len] = 0;
	}
	void Init(const char *key) {
		unsigned long len = 0;
		char*itr = (char*)key;
		while (*itr) { len++; itr++; }
		Init(key, len);
	}
	~hashstring() {
		free();
	}
	virtual long compare(const hashkey *otherKey) {
		char *kitr = m_key, *oitr = ((hashstring*)otherKey)->m_key;
		while (*kitr && (*kitr) == (*oitr)) { //will stop when *oitr == null too
			kitr++; oitr++;
		}
		if ((*kitr) == (*oitr)) return 0;
		return (*kitr) - (*oitr);
	}
	virtual unsigned long Key() const {
		unsigned long key = 0;
		char *itr = (char*)m_key;
		long i = 0;
		while (itr[i]) {
			key += itr[i] << (i & 3);
			if (!((i + 1) & 0x3)) {
				key ^= (i & 0x7) ? 0x55555555 : 0xaaaaaaaa;
			}
			i++;
		}
		return key;
	}
	virtual void free() { //free is called when hashkey is destructed
		if(m_key)
			delete[] m_key;
		m_key = 0;
	}
};

class hashmap {
	unsigned long tablesize;
	unsigned long long numitems;
	darray<hashkey*> **m_table;
public:
	unsigned long TableSize() { return tablesize; }
	void Resize(unsigned long tsize) {
		char olddata[sizeof(hashmap)];
		hashmap *old = (hashmap*)olddata;
		old->m_table = m_table;
		old->tablesize = tablesize;

		if (tsize == 0) {
			Clear();
			return;
		}

		unsigned long i;
		tablesize = 1;
		numitems = 0;
		while (tablesize < tsize) {
			tablesize <<= 1;
		}
		m_table = new darray<hashkey*>*[tablesize];
		for (i = 0; i < tablesize; i++) {
			m_table[i] = 0;
		}

		if (old->tablesize) {
			unsigned long i;
			for (i = 0; i < old->tablesize; i++) {
				if (old->m_table[i]) {
					unsigned long j;
					unsigned long size = old->m_table[i]->Size();
					for (j = 0; j < size; j++) {
						Append(old->m_table[i]->At(j));
					}
					delete old->m_table[i];
				}
			}
			delete [] old->m_table;
		}
	}
	hashmap(unsigned long tsize) {
		tablesize = 0;
		Resize(tsize);
	}
	~hashmap() {
		Clear();
	}
	unsigned long long Size() {
		return numitems;
	}
	void Clear() {
		unsigned long i;
		for (i = 0; i < tablesize; i++) {
			if (m_table[i]) {
				unsigned long j;
				for (j = 0; j < m_table[i]->Size(); j++) {
					delete m_table[i]->At(j);
				}
				delete m_table[i];
			}
			m_table[i] = 0;
		}
		if (tablesize) {
			delete[] m_table;
		}
		tablesize = 0;
		numitems = 0;
	}

	hashkey *operator [] (const hashkey *keystr) {
		return Find(keystr);
	}
	hashkey *Find(const hashkey *p_key, unsigned long p_instance = 0) {
		unsigned long key = p_key->Key()&(tablesize - 1);
		if (!m_table) {
			return 0;
		}
		darray<hashkey*> *a = m_table[key];
		if (!a) {
			return 0;
		}
		long size = a->Size();
		unsigned long instance = 0;
		long i = 0;
		while (i < size) {
			if (!a->At(i)->compare(p_key)) {
				if (instance == p_instance) {
					return a->At(i);
				}
				instance++;
			}
			i++;
		}
		return 0;
	}
	void Append(hashkey *p_data) {
		if (numitems+1 > tablesize * tablesize && tablesize < 65536) {
			Resize( tablesize+1);
		}
		unsigned long key = p_data->Key()&(tablesize - 1);
		if (!m_table[key]) {
			m_table[key] = new darray<hashkey*>(1);
		}
		numitems++;
		m_table[key]->push(p_data);

	}
	//Remove returns dtype so caller can delete, this only works where
	hashkey *Remove(const hashkey *p_key, unsigned long p_instance = 0) {
		unsigned long key = p_key->Key()&(tablesize - 1);
		darray<hashkey*> *a = m_table[key];
		if (!a) {
			return 0;
		}
		long size = a->Size();
		unsigned long instance = 0;
		long i = 0;
		while (i < size) {
			if (!a->At(i)->compare(p_key)) {
				if (instance == p_instance) {
					numitems--;
					if (i == 0) {
						hashkey *front = a->At(0);
						a->Crop(1, a->Size());
						return front;
					}
					hashkey *item = a->At(i);
					while (i < size-1) {
						a->At(i) = a->At(i + 1);
						i++;
					}
					a->pop();
					return item;
				}
				instance++;
			}
			i++;
		}
		return 0;
	}

	template <class dtype> friend class hashmap_itr;
};

template <class dtype>
class hashmap_itr {
	hashmap *m_map;
	unsigned long m_cur;
	unsigned long m_ti;
public:
	//SetMap also calls First()
	void Next() {
		m_cur++;
		if (m_ti < m_map->tablesize && m_map->m_table[m_ti] && m_cur<m_map->m_table[m_ti]->Size()) {
			return;
		}
		m_cur=0;
		m_ti++;
		while (m_ti < m_map->tablesize && (!m_map->m_table[m_ti] || !m_map->m_table[m_ti]->Size())) {
			m_ti++;
		}
	}
	void First(hashmap *p_map) {
		m_map = p_map;
		m_cur = 0;
		m_ti = 0;
		while (m_ti < m_map->tablesize && (!m_map->m_table[m_ti]|| !m_map->m_table[m_ti]->Size())) {
			m_ti++;
		}
	}
	dtype *Valid() {
		if (m_ti < m_map->tablesize) {
			if (m_map->m_table[m_ti] && m_cur < m_map->m_table[m_ti]->Size()) {
				return (dtype*)m_map->m_table[m_ti]->At(m_cur);
			}
		}
		return 0;
	}
	dtype *Item() {
		if (m_ti >= m_map->tablesize) {
			return 0;
		}
		return (dtype*)m_map->m_table[m_ti]->At(m_cur);
	}
};

