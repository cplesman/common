#pragma once

#include "stream.h"

class socket_stream : public stream {
public:
	socket_stream(void *id) { Init(id); }
	long getBytes(char *p_buffer, long p_numbytes);
	long putBytes(const char *p_buffer, long p_numbytes);
};


