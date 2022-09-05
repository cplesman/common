#include <string.h>
unsigned int hexCHToBin(unsigned char nib) {
	if ((nib >= 'a' && nib <= 'z'))
	{
		return nib - 'a' + 10;
	}
	else if ((nib >= 'A' && nib <= 'Z'))
	{
		return nib - 'A' + 10;
	}
	else
	{
		return nib - '0';
	}
}
unsigned char binCHToHex(unsigned char b) {
	if (b < 10)
	{
		return '0' + b;
	}
	return 'A' - 10 + b;
}
int hexStringToBin(unsigned char *result, const char *string) {
	int i;
	int len = (int)strlen(string);
	for (i = 0; i < len; i += 2) {
		result[i >> 1] = hexCHToBin(string[i + 1]);
		result[i >> 1] |= hexCHToBin(string[i]) << 4;
	}
	return i >> 1;
}
int hexStringToBinMax(unsigned char *result, const char *string, int max) {
	int i;
	int len = (int)strlen(string);
	int max2 = max * 2;
	for (i = 0; i < len && i<max2; i += 2) {
		result[i >> 1] = hexCHToBin(string[i + 1]);
		result[i >> 1] |= hexCHToBin(string[i]) << 4;
	}
	return i >> 1;
}
void binToHexString(char *result, const unsigned char *bin, int size) {
	int i;
	int len = size;
	for (i = len - 1; i >= 0; i--) {
		result[i * 2] = (char)binCHToHex(bin[i] >> 4);
		result[i * 2 + 1] = (char)binCHToHex(bin[i] & 0xf);
	}
	result[len * 2] = 0;
}
int movePastWhiteSpace(char *text) {
	char ch;
	int i = 0;
	do {
		ch = text[i];
		i++;
	} while ((ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t'));
	return i-1;

}