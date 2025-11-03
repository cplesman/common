#pragma once
#define BASE64_ERROR_TOOBIG  -1
#define BASE64_ERROR_INVALIDDATA  -2
int toBase64(char *output, const unsigned char *input, int inputLen);
int fromBase64(unsigned char *output, const char *input, int inputLen);
