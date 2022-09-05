#pragma once

#define HTTP_HEADER_TYPE_JSON		1
#define HTTP_HEADER_TYPE_TOKENUSER	2 //SPECIAL USER

#define HTTP_HEADER_VERB_GET		0
#define HTTP_HEADER_VERB_POST		1
#define HTTP_HEADER_VERB_PATCH		2

#include "stream/stream.h"
#include "darray.h"
#include "json.h"

class http_header {
public:
	darray<char> lineBuffer;
	darray<char> path;
	jsonobj query;
	jsonobj headers;
	http_header() {

	}
	~http_header() {
		lineBuffer.Resize(0);
	}
	char verb;
	char type;
	void ToLower(darray<char> *line) {
		long size = (long)line->Size();
		long i;
		for (i = 0; i < size; i++) {
			char ch = line->At(i);
			if (ch >= 65 && ch <= 90) {
				line->At(i) = ch + (97 - 65);
			}
		}
	}
	long Load(stream *buf);
	long LoadHeaderLine(stream *buf,darray<char> *line);
	long LoadPath(stream *buf);
	long ParseQuery(const char *query);
};
