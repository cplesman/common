#include "http.h"
#include "stream/stream.h"
#include "darray.h"
#include <string.h>

long http_header::Load(stream *buf) {
	headers.Free();
	query.Free();
	char verbText[4];
	long err = buf->GetBytes(verbText, 4);
	if (err <= 0) {
		return err;
	}
	if (!strncmp(verbText, "GET", 3)) {
		verb = HTTP_HEADER_VERB_GET;
	}
	else if (!strncmp(verbText, "POST", 4)) {
		verb = HTTP_HEADER_VERB_POST;
		err = buf->GetBytes(verbText, 1); //goto path
	}
	else {
		if (strncmp(verbText, "PATC", 4)) {
			return -1009;
		}
		verb = HTTP_HEADER_VERB_PATCH;
		err = buf->GetBytes(verbText, 2); //goto path
	}
	if (err <= 0) {
		return err;
	}

	err = LoadPath(buf);
	if (err <= 0) return err;

	type = 0;
	while ((err = LoadHeaderLine(buf, &lineBuffer)) > 0) {
		//ToLower(&lineBuffer);
		const char *lineData = lineBuffer.GetData();
		long itr = 0;
		while (lineData[itr] == ' ' || lineData[itr] == '\t' ) itr++;
		long startKey = itr;
		while (lineData[itr] != ':' && lineData[itr]!=' ' && lineData[itr] != '\t' && lineData[itr]) itr++;
		long klen = itr-startKey;
		while (lineData[itr] == ':' || (lineData[itr] == ' ' || lineData[itr] == '\t') ) itr++;
		long startValue = itr;
		while (lineData[itr] != ' ' && lineData[itr] != '\t' && lineData[itr]) itr++;
		long vlen = itr-startValue;

		if (vlen <= 0 || klen <= 0) {
			continue; //invalid header?
		}

		headers.AppendText(lineData + startKey, klen, lineData + startValue, vlen);
	}
	return err >= 0 ? 1 : -1;
}

long http_header::LoadPath(stream *buf) {
	long err = LoadHeaderLine(buf, &path);
	if (err <= 0) {
		return err;
	}
	err = 0;
	for (; err < (long)path.Size(); err++) {
		if (path[err] == ' ') {
			path.Crop(0, err);
			path[err] = 0;
			break;
		}
		if (path[err] == '?') {
			ParseQuery(path.GetData() + err + 1);
			path.Crop(0, err); //remove query from path
			path[err] = 0;
			break;
		}

	}
	return err;
}

long http_header::ParseQuery( const char *queryText) {
	long i = 0;
	char key[64];
	char value[192];
	while (queryText[i] != ' '&&queryText[i] && queryText[i] != '\r') {
		const char *start = queryText + i;
		long i2 = 0;
		while (queryText[i] != ' ' && queryText[i] != '='&&queryText[i] && queryText[i] != '&' && i2 < 63) {
			key[i2] = queryText[i];
			i2++;
			i++;
		}
		key[i2] = 0;
		if (queryText[i] != '=') {
			return -1;
		}
		i++;
		i2 = 0;
		while (queryText[i] != ' '  && queryText[i] != '='&&queryText[i] && queryText[i] != '&' && i2 < 191) {
			value[i2] = queryText[i];
			i2++;
			i++;
		}
		value[i2] = 0;
		query.AppendText(key, value);
	}
	return 0;
}

long http_header::LoadHeaderLine(stream *buf, darray<char> *line) {
	char ch;
	long total = 0;
	line->Empty();
	line->CheckSize(128);
	do {
		long err = buf->GetBytes(&ch, 1);
		if (err <= 0) {
			return err;
		}
		if (ch == '\r') { //ignore all carriage returns
			continue;
		}
		if (ch == '\n') {
			line->push(0); //null terminate line, total will not include null
			break;
		}
		total += err; //will include null character
		if (line->AllocSize() < line->Size() + 2) {
			line->Resize(line->Size() + 32);
		}
		line->push(ch);
	} while (ch);
	return total;
}

