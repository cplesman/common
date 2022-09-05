#pragma once

#include <string.h>

template <class dtype>
class stack {
protected:
	dtype *m_data;
	unsigned long m_totalsize; //must be power of 2
	unsigned long m_next;
	unsigned long m_front;
	unsigned long index(unsigned long p_i) {
		return (m_front + p_i)&(m_totalsize-1);
	}
	void resize(unsigned long p_newsize/*power of 2*/) {
		//assume new never fails
		//size^2 >= p_newsize
		dtype *nArray = p_newsize ? (new dtype[p_newsize]) : 0;
		if (m_data) {
			unsigned long i;
			unsigned long smallerSize = p_newsize;
			if (Size() < p_newsize) smallerSize = Size();
			for (i = 0; i < smallerSize; i++) {
				nArray[i] = m_data[index(i)]; //copy byte for byte
			}
			delete [] m_data;
			m_next = smallerSize; //call before changing front
		}
		else {
			m_next = 0;
		}
		m_front = 0;
		m_totalsize = p_newsize;
		m_data = nArray;
	}
public:
	darray() :darray(0) {}
	darray(unsigned long p_newsize) {
		m_data = 0;
		m_front = 0;
		m_next = 0;
		m_totalsize = 0;
		Resize(p_newsize);
	}
	~darray() {
		if (m_data) delete[] m_data;
	}

	void Resize(unsigned long size) {
		unsigned long size2 = 0;
		if (size != Size() || m_front!=0) {
			if (size > 0) {
				size2 = 1;
				while (size2 < size) size2 <<= 1;
				resize(size2);
			}
			else {
				resize(0);
			}
		}
	}

	dtype &At(unsigned long p_i) {
		return m_data[index(p_i)];
	}

	void Empty() {
		m_next = m_front = 0;
	}
	unsigned long Size() {
		return m_next - m_front;
	}
	unsigned long AllocSize() {
		return m_totalsize;
	}

	const dtype *GetData() { return m_data; }
	void SetData(const char*p_data, unsigned long size) {
		SetSize(size);
		memcpy(m_data, p_data, size / sizeof(dtype));
	}
	void SetSize(unsigned long size) {
		Empty();
		if (size > m_totalsize) {
			Resize(size);
		}
		m_next = size;
	}

	void push(dtype p_data) {
		unsigned long size = m_totalsize;
		if (size == Size()) { //reached total size?
			resize(m_totalsize? m_totalsize*2 : 1);
		}
		unsigned long next = m_next;
		m_data[next&(m_totalsize-1)] = p_data;
		m_next++; //always update at the end for multitasking
	}
	dtype pop() {
		unsigned long next = m_next;
		dtype r = m_data[next &(m_totalsize-1)];
		m_next--; //always update at the end for multitasking
		return r;
	}
	dtype front() {
		unsigned long front = m_front;
		dtype r = m_data[front & (m_totalsize-1)/*mask*/];
		m_front++;
		return r;
	}

};