#pragma once

template <class dataType>
class reference_link {
public:
	dataType *m_ref;
	reference_link *m_next;
};

template <class dataType>
class safe_ptr {
private:
	dataType *m_ptr;
#ifdef _DEBUG
	reference_link *m_references;
#endif
public:
	safe_ptr(dataType *ptr) {
		m_ptr = ptr;
		m_references = 0;
	}
	~safe_ptr() {
		if (m_references) {
			throw ("pointer is still being referenced");
		}
	}
	reference_link *findRef(dataType *ref,reference_link **prev=0) {
		reference_link *itr = m_references;
		while (itr) {
			if (itr->m_ref = ref) {
				return itr;
			}
			if (prev) {
				*prev = itr;
			}
			itr = itr->m_next;
		}
		return 0;
	}
	void create_ref(dataType *ref) {
		if (findRef(ref)) {
			throw("reference already in use");
		}
		else {
			reference_link *ref = new reference_link;
			ref->m_next = m_references;
			ref->m_ref = ref;
		}
	}
	void free_ref(dataType *ref) {
		reference_link *f, *prev = 0;
		if (!(f = findRef(ref,&prev)) ){
			throw("reference not found or already freed");
		}
		if (prev) {
			prev->m_next = f->next;
		}
		else {
			m_references = f->next;
		}
	}
};