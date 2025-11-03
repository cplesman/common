#pragma once

template <class dtype>
class iarray_item {
public:
	union {
		unsigned long nextFree;
		dtype data;
	};
};

//super fast dynamic array
template <class dtype>
class iarray {
public:
	unsigned long nextFree;
	unsigned long lastI;
	unsigned long Size() { return lastI; }
	iarray_item<dtype> *data;
	iarray() {
		lastI = nextFree = 0;
	}
	~iarray() {
		if(lastI)
			delete[] data;
	}
	void Resize(unsigned long size) {
		if (size) {
			unsigned long i = 0;
			iarray_item<dtype> *nd = new iarray_item<dtype>[size];
			for (; i < lastI; i++) {
				nd[i] = data[i];
			}
			for (; i < size; i++) {
				nd[i].nextFree = i + 1;
			}
			if (lastI) {
				delete[] data;
			}
			lastI = size;
		}
		else if (lastI) {
			delete data;
			data = 0;
			nextFree =lastI = 0;
		}
	}

	dtype &operator [](unsigned long idx) {
		return data[idx].data;
	}
	unsigned long Alloc() {
		if (nextFree == lastI) {
			Resize( (lastI + 1)*2 );
		}
		unsigned long cur = nextFree;;
		nextFree = data[nextFree].nextFree;
		return cur;
	}
	void Free(unsigned long idx) {
		data[idx].nextFree = nextFree;
		nextFree = idx;
	}

};
