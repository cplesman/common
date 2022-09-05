#include "json.h"
#include "hexdata.h"
#include <string.h>
#include <stdlib.h>

int JSON_parseVal(jsonobj *obj, char lastch, stream *buf);

long jsonobj::Load(stream *buf) {
	int err;
	char ch;

	err = JSON_movepastwhite(buf);
	if (err < 0) { return err; }
	ch = (char)err;

	if (name) {
		delete[] name;
	}
	name = 0; //no name object

	err = JSON_parseVal(this, ch, buf);
	if (err < 0) { JSON_free(this); return err; }

	return 0;
}

int JSON_parseString_iterateNumber(char *result, stream *buf);
int JSON_parseString_iterateQuote(char *result, char quote, stream *buf);
int JSON_createnext(jsonobj **retObj, char openBracket, stream *buf);
int JSON_parseString(jsonobj **result, char lastch, stream *buf);

int JSON_movepastwhite(stream *buf) {
	char ch;
	do {
		int err = buf->GetBytes(&ch, 1);
		if (err < 0) return err;
	} while ((ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t'));
	return ch;
}
jsonobj *JSON_allocate() {
	jsonobj *obj = new jsonobj;
	return obj;
}
void JSON_free(jsonobj *obj) {
	delete obj;
}
int JSON_parseVal(jsonobj *obj, char lastch, stream *buf) {
	int err;
	char ch = lastch;
	//determine type
	if (ch == '{') { //obj in obj
		int count = 0;
		obj->type = JSON_OBJ;
		//obj->valobj = 0; already zero'd
		jsonobj **cur = &obj->valobj;

		do {
			err = JSON_createnext(cur, ch/* '{' or ',' */,  buf);
			if (err < 0) {  return err; }
			if (!(*cur) && ((char)err) == '}'/*JSON_EMPTY*/) {
				break; // count==0
			}
			err = JSON_movepastwhite(buf); //also gets next ch
			if (err < 0) { return err; }
			ch = (char)err;

			if (ch != '}' && ch != ',') {  return -4; } //invalid obj

			//if (ch == ',') {
			//	err = JSON_movepastwhite(buf);
			//	if (err < 0) { return err; }
			//	ch = (char)err;
			//}

			cur = &((*cur)->next);
			count++;
		} while (ch != '}');

		return '}'; //ch
	}
	else if (ch == '[') {
		int count = 0;
		jsonobj **cur = &obj->valobj;
		obj->type = JSON_ARRAY;

		err = JSON_movepastwhite(buf);
		if (err < 0) { return err; }
		ch = (char)err;

		//if ch==']' prev will be null, count == 0
		while (ch != ']') {
			err = JSON_parseString(cur, ch, buf);
			if (err < 0) {  return err; }
			ch = (char)err;

			err = JSON_movepastwhite(buf);
			if (err < 0) { return err; }
			ch = (char)err;

			if (ch != ']' && ch != ',') {  return -4; } //invalid obj

			if (ch == ',') {
				err = JSON_movepastwhite(buf); //move past ',' and whitespace
				if (err < 0) { return err; }
				ch = (char)err;
			}

			cur = &((*cur)->next);
			count++;

			//if (ch == ',') {
			//	err = JSON_movepastwhite(buf); //white space after ','
			//	if (err < 0) { return err; }
			//	ch = (char)err;
			//}
		}

		return ']';//ch
	}
	else if (ch == '\'' || ch == '\"') {
		obj->type = JSON_TEXT;
		err = JSON_parseString_iterateQuote(0, ch, buf);
		if (err < 0) { return err; }
		obj->val = new char[/*size with null*/err];
		if (err < 0) { return err; }
		err = JSON_parseString_iterateQuote(obj->val, ch, buf);
		if (err < 0) { return err; }
		return ch;
	}
	else if ((ch >= '0'&& ch <= '9')||ch=='-') {
		//is number
		obj->type = JSON_NUMBER;
		char numstr[32];
		numstr[0] = ch;
		err = JSON_parseString_iterateNumber(numstr+1, buf);
		if (err < 0) return err;
		obj->valnum = atof(numstr);
		return numstr[err];
	}

	return -4; //not a valid obj
}

int JSON_createnext(jsonobj **retObj, char openBracket, stream *buf) {
	char ch;
	int err;

	//after '{' or ','
	err = JSON_movepastwhite( buf);
	if (err < 0) return err;
	ch = (char)err;

	if (openBracket == '{' && ch == '}') {
		//empty PARENT obj
		return ch;
	}

	if (ch != '\'' && ch != '\"') {
		return -4; //invalid json obj
	}

	jsonobj *obj = new jsonobj;
	if (!obj) return -8;

	err = JSON_parseString_iterateQuote(0, ch,buf);
	if (err < 0) { JSON_free( obj); return err; }
	obj->name = new char[err];
	if(!obj->name) { JSON_free(obj); return -8; }
	err = JSON_parseString_iterateQuote(obj->name, ch, buf);
	if (err < 0) {JSON_free( obj);return err;}

	err = JSON_movepastwhite(buf);
	if (err < 0) {JSON_free(obj);return err;}
	ch = (char)err;

	if (err != ':') {JSON_free(obj);return -4;} //invalid obj

	err = JSON_movepastwhite(buf);
	if (err < 0) {JSON_free( obj);return err;}
	ch = (char)err;

	err = JSON_parseVal(obj, ch, buf);
	if (err < 0) {
		JSON_free( obj); return err;
	}
	
	*retObj = obj;
	return err;
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
	if (result) *(result + size) = 0; //null terminate
	return size;
}
int JSON_parseString_iterateQuote(char *result, char quote, stream *buf) {
	int size = 0;
	char ch = 0;
	while (ch != quote) {
		int err;
		if (result) {
			err = buf->GetBytes( &ch, 1);
		}
		else {
			err = buf->PeekBytes(size, &ch, 1);
		}
		if (err < 0) return err;
		char ch2 = ch;
		if (ch == '\\'){ //escape
			int err;
			if (result) {
				err = buf->GetBytes( &ch2, 1);
			}
			else {
				err = buf->PeekBytes(size, &ch2, 1);
			}
			if (err < 0) return err;
			if (ch2 == 'r') ch2 = '\r';
			else if (ch2 == 'n')ch2 = '\n';
		}
		if (ch != quote){
			if(result) *(result+size) = ch2;
			size++;
		}
	}
	if(result) *(result+size)= 0; //null terminate
	size++;
	return size;
}

int JSON_parseString(jsonobj **result, char lastch, stream *buf) {
	int err;
	char ch = lastch;

	jsonobj *obj = JSON_allocate();
	if (!obj) { return -8;/*out of mem*/ }

	obj->name = 0; //no name object


	err = JSON_parseVal(obj, ch, buf);
	if (err < 0) { JSON_free( obj); return err; }

	*result = obj;

	return 0;
}

int JSON_parseData(jsonobj **result, stream *buf) {
	int err;
	char ch;

	err = JSON_movepastwhite(buf);
	if (err < 0) { return err; }
	ch = (char)err;

	return JSON_parseString(result, ch, buf);
}

jsonobj **JSON_findKeyPtr(jsonobj *parent, const char *key) {
	jsonobj **itr = &parent->valobj;
	while ((*itr)) {
		if ((*itr)->name) {
			if (!strcmp((*itr)->name, key)) {
				return itr;
			}
		}

		itr = &(*itr)->next;
	}
	return 0;
}
jsonobj **JSON_getIndexPtr(jsonobj *parent, const int i) {
	jsonobj **itr = &parent->valobj;
	int count = 0;
	while ((*itr)) {
		if (count == i) return itr;
		count++;
		itr = &(*itr)->next;
	}
	return 0;
}
jsonobj *JSON_findKey(jsonobj *parent, const char *key) {
	jsonobj *itr = parent->valobj;
	while ((itr)) {
		//TODO: use a hash table
		if ((itr)->name) {
			if (!strcmp((itr)->name, key)) {
				return itr;
			}
		}

		itr = itr->next;
	}
	return 0;
}
jsonobj *JSON_getIndex(jsonobj *parent, const int i) {
	jsonobj *itr = parent->valobj;
	int count = 0;
	while ((itr)) {
		if (count == i) return itr;
		count++;
		itr = itr->next;
	}
	return 0;
}

jsonobj *JSON_allocateTextObj( long keySize, long valSize) {
	jsonobj *child = JSON_allocate();
	if (!child)
		return 0;
	child->type = JSON_TEXT;
	int alloclen = keySize + 1;
	int alloctextlen = valSize + 1;
	child->name/*key*/ = new char[alloclen];
	if (!child->name) {
		JSON_free(child);
		return 0;
	}
	child->val = new char[alloctextlen];
	if (!child->val) {
		JSON_free(child); //will free name as well
		return 0;
	}
	return child;
}
int JSON_prependObj(jsonobj *parent, jsonobj *child) {
	if (!child) return JSON_ERROR_OUTOFMEMORY;
	child->next = parent->valobj; //link to begining
	parent->valobj = child;
	return 0;
}

void JSON_removeObj( jsonobj *parent, const char *key) {
	jsonobj **obj = JSON_findKeyPtr(parent, key);
	if (obj) {
		jsonobj *save = *obj;
		*obj = (*obj)->next;
		JSON_free( save);
	}
}

long JSON_countChildren(jsonobj*parent) {
	jsonobj *itr = parent->valobj;
	long count = 0;
	while (itr) {
		count++;
		itr = itr->next;
	}
	return count;
}

