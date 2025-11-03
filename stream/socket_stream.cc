#include "socket_stream.h"

#include "../server/client.h"


long socket_stream::getBytes(char *p_buffer, long  p_numbytes) { //gets the next (upto)256 bytes, returns number of bytes read, or negative for error
	int iResult = CyaSSL_read(((client*)m_streamid)->ssl, p_buffer, p_numbytes);
	if (iResult <= 0) { // 0 == closed connection ( -1 on return )
		return iResult == 0 ? (-1) : (-2);
	}
	return iResult;
}
long socket_stream::putBytes(const char *p_buffer, long  p_numbytes) { //gets the next (upto)256 bytes, returns number of bytes read, or negative for error
	int iResult = CyaSSL_write(((client*)m_streamid)->write_ssl, p_buffer, p_numbytes);
	if (iResult <= 0) { return iResult == 0 ? -1 : -2; }
	return iResult;
}

