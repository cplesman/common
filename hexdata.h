#pragma once

int hexStringToBin(unsigned char *result, const char *string);
int hexStringToBinMax(unsigned char *result, const char *string, int max/*max size of result*/);
void binToHexString(char *result, const unsigned char *bin, int size/*size of bin*/);
int movePastWhiteSpace(char *text);
