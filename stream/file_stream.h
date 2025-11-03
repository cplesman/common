#pragma once

#include "stream.h"

#include <stdio.h>

class file_stream : public stream {

	FILE *f;
public:
	file_stream(const char *filename, const char *mode) {
		f = fopen(filename, mode);
	}
	~file_stream() {
		if(f)
		fclose(f);
	}
	long getBytes(char *p_buffer, long p_numBytes) {
		return (long)fread(p_buffer, 1, p_numBytes, f);
	}
	long putBytes(const char *p_buffer, long p_numBytes) {
		return (long)fwrite(p_buffer, 1, p_numBytes, f);
	}
};