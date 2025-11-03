#pragma once

//#include <string.h>

template <class dtype>
class darray {
protected:
	dtype *m_data;
	unsigned long m_totalsize; 
	unsigned long m_size;
public:
	darray() :darray(0) {}
	darray(unsigned long p_newsize) {
		m_data = 0;
		m_size = 0;
		m_totalsize = 0;
		Resize(p_newsize);
	}
	~darray() {
		if (m_data) delete[] m_data;
	}

	int Resize(unsigned long p_newsize) {
		//assume new never fails
		//size^2 >= p_newsize
		dtype *nArray = p_newsize ? (new dtype[p_newsize]) : 0;
		if (!nArray) return -1;
		if (m_data) {
			unsigned long i;
			unsigned long smallerSize = p_newsize;
			if (m_size < p_newsize) smallerSize = m_size;
			for (i = 0; i < smallerSize; i++) {
				nArray[i] = m_data[i]; //copy byte for byte
			}
			delete[] m_data;
			m_size = smallerSize;
		}
		else {
			m_size = 0;
		}
		m_totalsize = p_newsize;
		m_data = nArray;
	}

	dtype &At(unsigned long p_i) {
		return m_data[p_i];
	}
	dtype &operator [](unsigned long p_i) {
		return m_data[p_i];
	}

	void Empty() {
		m_size = 0;
	}
	unsigned long Size() {
		return m_size;
	}
	unsigned long AllocSize() {
		return m_totalsize;
	}

	void Crop(unsigned long start, unsigned long end) {
		if (start == 0) {
			m_size = end;
		}
		else {
			unsigned long i;
			unsigned long len = end - start;
			for (i = 0; i < len; i++) {
				m_data[i] = m_data[i + start];
			}
			m_size = len;
		}
	}

	void Copy(const darray<dtype> *src) {
		if (m_data) {
			delete [] m_data;
		}
		m_totalsize = src->m_size;
		m_size =src->m_size;
		if (src->m_size) {
			m_data = new dtype[src->m_size];
			int i;
			for (i = 0; i < src->m_size; i++) {
				m_data[i] = src->m_data[i];
			}
		}
	}

	const dtype *GetData() { return m_data; }
	void SetData(const char*p_data, unsigned long size) {
		unsigned long s = size / sizeof(dtype);
		SetSize(s);
		memcpy(m_data, p_data, s * sizeof(dtype));
	}
	void SetSize(unsigned long size) {
		if (size > m_totalsize) {
			Resize(size);
		}
		m_size = size;
	}
	int CheckSize(unsigned long increment) {
		if (m_size + increment > m_totalsize) {
			return Resize(m_size + increment);
		}
		return 0;
	}

	void push(dtype p_data) {
		unsigned long size = m_totalsize;
		if (size == Size()) { //reached total size?
			Resize(m_totalsize ? m_totalsize * 2 : 1);
		}
		unsigned long next = m_size;
		m_data[next] = p_data;
		m_size++; //always update at the end for multitasking
	}
	dtype pop() {
		m_size--;
		return m_data[m_size];
	}

};
