#include "string.h"
#ifdef _DEBUG
#include <assert.h>
#endif // _DEBUG

int sauce_strlen(const char *pString){
	int i = 0;
	while (pString[i]) i++;
	return i;
}
int sauce_strlen_max(const char *pString, int maxLen){
	int i = 0;
	while (pString[i] && i < maxLen) i++;
	return i;
}

char *sauce_strcpy(char *pDest, const char *pSource){
	int i = 0;
	while (pSource[i]){
		pDest[i] = pSource[i];
		i++;
	}
	pDest[i] = 0;

	return pDest;
}

char *sauce_strncpy(char *pDest, const char *pSource, int pLength){
	int i = 0;
	while (i<pLength){
		pDest[i] = pSource[i];
		i++;
	}
	pDest[i] = 0;

	return pDest;
}

char *sauce_strcat(char *pDest, char *pSource){
	int i = sauce_strlen(pDest);
	int i2 = 0;
	while (pSource[i2]){
		pDest[i + i2] = pSource[i2];
		i2++;
	}
	pDest[i + i2] = 0;

	return pDest;
}
char *sauce_strncat(char *pDest, char *pSource, int pLen){
	int i = sauce_strlen(pDest);
	int i2 = 0;
	while (i2<pLen ){
		pDest[i + i2] = pSource[i2];
		i2++;
	}
	pDest[i + i2] = 0;

	return pDest;
}

int sauce_strcmp(const char *p1, const char *p2){
	int i = 0;

	while ((p1[i] | p2[i]) && p1[i] == p2[i]){
		i++;
	}

	return p1[i] - p2[i];
}

int sauce_strncmp(const char *p1, const char *p2, int pLen){
	int i = 0;

	while( i < pLen && (p1[i] | p2[i]) && p1[i] == p2[i] ){
		i++;
	}

	return i==pLen ? 0 : (p1[i] - p2[i]);
}


char *sauce_tolower(char *pDest, char *pSource){
	int i = 0;
	while (pSource[i]){
		int ch = pSource[i];
		if (pSource[i] >= 'A' && pSource[i] <= 'Z')
			ch += ('a' - 'A');
		pDest[i] = (char)ch;
		i++;
	}
	pDest[i] = 0;

	return pDest;
}
char *sauce_tolower_n(char *pDest, char *pSource, int n){
	int i = 0;
	while (pSource[i] && i < n){
		int ch = pSource[i];
		if (pSource[i] >= 'A' && pSource[i] <= 'Z')
			ch += ('a' - 'A');
		pDest[i] = (char)ch;
		i++;
	}
	pDest[i] = 0;

	return pDest;
}
char *sauce_toupper(char *pDest, char *pSource){
	int i = 0;
	while (pSource[i]){
		int ch = pSource[i];
		if (pSource[i] >= 'a' && pSource[i] <= 'z')
			ch += ('A' - 'a');
		pDest[i] = (char)ch;
		i++;
	}
	pDest[i] = 0;

	return pDest;
}

int isIntegerText(char *str){
	int nLen = 0;
	int isHex = 0;
	if (str[nLen] == '0' && (str[nLen + 1] == 'x' || str[nLen + 1] == 'X')) {
		isHex = 1;
		nLen += 2; //hex case
	}
	while (((str[nLen] <= '9' && str[nLen] >= '0') || (isHex && ((str[nLen] >= 'A' && str[nLen] <= 'F') || (str[nLen] >= 'a'&&str[nLen] <= 'f')))) && str[nLen]){
		nLen++;
	}
	if (str[nLen] == '.' || isTextCharacter(str[nLen])){ //float type
		return 0;
	}
	return nLen;
}
int isFloatText(char *str, int pCheckDoublePrecision){
	int nLen = 0;
	int couldBeDouble = 0;
	while (isNumberCharacter(str[nLen])){
		nLen++;
	}
	if (str[nLen] == '.' && nLen){
		couldBeDouble = 1;
		nLen++;
		while (isNumberCharacter(str[nLen])){
			nLen++;
		}
	}
	if (str[nLen] == 'e' || str[nLen] == 'E'){
		couldBeDouble = 1;
		nLen++;
		if (str[nLen] == '-'){
			nLen++;
			while (isNumberCharacter(str[nLen])){
				nLen++;
			}
		}
		else {
			if (str[nLen] == '+') nLen++;
			while (isNumberCharacter(str[nLen])){
				nLen++;
			}
		}
	}
	if (str[nLen] != 'f' && (!couldBeDouble || !(pCheckDoublePrecision))) {
		return 0;
	}
	return nLen;
}
int isNumberText(char *str){	//includes double and integer, returns length
	int nLen = 0;
	if (!isNumberCharacter(str[nLen])) return 0;
	int isHex = 0;
	if (str[nLen] == '0' && (str[nLen + 1] == 'x' || str[nLen + 1] == 'X')){
		isHex = 1;
		nLen += 2;
	}
	while (((str[nLen] <= '9' && str[nLen] >= '0') || (isHex && ((str[nLen] >= 'A' && str[nLen] <= 'F') || (str[nLen] >= 'a'&&str[nLen] <= 'f')))) && str[nLen]){
		nLen++;
	}
	if (!isHex){
		if (str[nLen] == '.' && nLen){
			nLen++;
			while (isNumberCharacter(str[nLen])){
				nLen++;
			}
		}
		if (str[nLen] == 'e' || str[nLen] == 'E'){
			nLen++;
			if (str[nLen] == '-'){
				nLen++;
				while (isNumberCharacter(str[nLen])){
					nLen++;
				}
			}
			else {
				if (str[nLen] == '+') nLen++;
				while (isNumberCharacter(str[nLen])){
					nLen++;
				}
			}
		}
		if (str[nLen] == 'f' || str[nLen] == 'F')/*float*/
			nLen++;
	}
	return nLen;
}

#define GETELEMENTTABLEHASHKEY(ch0,ch1)	(((ch0)+(ch1<<2))&0xf)
#define MAKECHAR3(ch0,ch1,ch2)		(ch0|(ch1<<8)|(ch2<<16))
struct GetElementTableItem{
	unsigned char m_ch0, m_ch1, m_ch2, m_next;
};
struct GetElementTableItem gGetElementNameLengthElementTable[] = {
	//0 :   <,=,0 | ,=,0
	//1 : -,-,0    =,=,0 - ,=,0
	//2 : >,=,0 : , : ,0 ^ ,=,0
	//3 : / ,=,0
	//4 :
	//5 : -,>,0    !,=,0
	//6 :   >,>,0 * , / ,0    >,>,=
	//7 :   +,+,0 / ,*,0
	//8 :
	//9 : %,=,0
	//10 : &,=,0
	//11 : / , / ,0
	//12 : <,<,0 | , | ,0    <,<,=
	//13 :
	//14 : &,&,0 * ,=,0
	//15 : +,=,0
{'<','=',0,16},
{'-','-',0,17},
{'>','=',0,19},
{'/','=',0,0},
{0,0,0,0},
{'-','>',0,21},
{'>','>',0,22},
{'+','+',0,24},
{ 0,0,0,0 },
{'%','=',0,0},
{'&','=',0,0},
{'/','/',0,0},
{'<','<',0,25},
{ 0,0,0,0 },
{'&','&',0,27},
{'+','=',0,0},
{ '|','=',0,0 }, //16
{ '=','=',0,18 },
{ '-','=',0,0 },
{ ':',':',0,20 },
{ '^','=',0,0 },
{ '!','=',0,28},
{ '*','/',0,0/*23*/ },
{ '>','>','=',0 }, //23
{ '/','*',0,0 },
{ '|','|',0,0/*26*/ }, //25
{ '<','<','=',0 },
{ '*','=',0,0 }, //27
{ '=', '>', 0,0}   //lambda
};


//int gIsENLTableInit = 0;
//void InitGENLTable() {
//	gIsENLTableInit = 1;
//
//	struct GetElementTableItem getElementNameLengthElementTable[] = {
//		{ ('-') ,('-') ,0 ,0 },
//	{ ('+') ,('+') ,0 ,0 },
//
//	{ ('<') ,('<') ,0 ,0 },
//	{ ('>') ,('>') ,0 ,0 },
//
//	{ ('-') ,('>') ,0 ,0 },
//	//comments:
//	{ ('/') ,('/') ,0 ,0 },
//	{ ('/') ,('*') ,0 ,0 },
//	{ ('*') ,('/') ,0 ,0 },
//	//comparison:
//	{ ('|') ,('|') ,0 ,0 },
//	{ ('&') ,('&') ,0 ,0 },
//	{ ('!') ,('=') ,0 ,0 },
//	{ ('=') ,('=') ,0 ,0 },
//	{ ('>') ,('=') ,0 ,0 },
//	{ ('<') ,('=') ,0 ,0 },
//
//	{ (':') ,(':') ,0 ,0 },
//	//asignment:
//	{ ('^') ,('=') ,0 ,0 },
//	{ ('&') ,('=') ,0 ,0 },
//	{ ('|') ,('=') ,0 ,0 },
//	{ ('+') ,('=') ,0 ,0 },
//	{ ('-') ,('=') ,0 ,0 },
//	{ ('/') ,('=') ,0 ,0 },
//	{ ('*') ,('=') ,0 ,0 },
//	{ ('%') ,('=') ,0 ,0 },
//	{ ('<') ,('<') ,('=') ,0 },
//	{ ('>') ,('>') ,('=') ,0 }
//	};
//	int i;
//	int nextstoreloc = 16;
//	for (i = 0; i < sizeof(getElementNameLengthElementTable) / sizeof(struct GetElementTableItem);i++) {
//		int k = (getElementNameLengthElementTable[i].m_ch0 + (getElementNameLengthElementTable[i].m_ch1<<2))&0xf;
//		if (!gGetElementNameLengthElementTable[k].m_ch0) {
//			gGetElementNameLengthElementTable[k] = getElementNameLengthElementTable[i];
//		}
//		else {
//			char next = k;
//			while (gGetElementNameLengthElementTable[next].m_next) {
//				next = gGetElementNameLengthElementTable[next].m_next;
//			} 
//			gGetElementNameLengthElementTable[next].m_next = (char)nextstoreloc;
//			gGetElementNameLengthElementTable[nextstoreloc] = getElementNameLengthElementTable[i];
//			nextstoreloc++;
//		}
//	}
//}

int GetElementNameLength(char *pElementName){
//#ifdef _DEBUG
//	assert(gIsENLTableInit);
//#endif // _DEBUG
	if (!*pElementName) {
		return 0;
	}
	int count = 0;
	int nlen;
	if (nlen = isNumberText(pElementName)) {
		if (pElementName[nlen] == '$') //special character
			count++;
		return count + nlen;
	}
	else {
		if (isTextCharacter((*pElementName)) || (*pElementName) == '_') {
			do {
				count++;
			} while ((isTextCharacter((pElementName[count])) || isNumberCharacter((pElementName[count])) || (pElementName[count]) == '_'));
			if (pElementName[count] == '$') //special character
				count++;
			return count;
		}
		else {
			int e0 = *pElementName;
			int e1 = 0, e2 = 0;
			if (e0)
				e1 = *(pElementName + 1);
			//if (e1)
			//	e2 = *(pElementName + 2);
			unsigned long chseq3 = (*((unsigned long*)pElementName)) & 0xffffff;

			//place all 3 length objects here:
			if (MAKECHAR3('<', '<', '=') == chseq3 || MAKECHAR3('>', '>', '=') == chseq3
				//(e0 == '<' && e1 == '<' && e2 == '=') ||
				//(e0 == '>' && e1 == '>' && e2 == '=')
				) {
				return 3;
			}

			//place all 2 length objects here:
			unsigned short chseq2 = chseq3 & 0xffff;
			unsigned char k = GETELEMENTTABLEHASHKEY(e0, e1);
			unsigned char next = GETELEMENTTABLEHASHKEY(e0, e1);
			do {
				if (*((unsigned short*)&gGetElementNameLengthElementTable[next]) == chseq2) {
					return 2;
				}
				next = gGetElementNameLengthElementTable[next].m_next;
			} while (next);

			//if (
			//	(e0 == '-' && e1 == '-') ||
			//	(e0 == '+' && e1 == '+') ||

			//	(e0 == '<' && e1 == '<') ||
			//	(e0 == '>' && e1 == '>') ||

			//	(e0 == '-' && e1 == '>') ||
			//	//comments:
			//	(e0 == '/' && e1 == '/') ||
			//	(e0 == '/' && e1 == '*') ||
			//	(e0 == '*' && e1 == '/') ||
			//	//comparison:
			//	(e0 == '|' && e1 == '|') ||
			//	(e0 == '&' && e1 == '&') ||
			//	(e0 == '!' && e1 == '=') ||
			//	(e0 == '=' && e1 == '=') ||
			//	(e0 == '>' && e1 == '=') ||
			//	(e0 == '<' && e1 == '=') ||

			//	(e0 == ':' && e1 == ':') ||
			//	//asignment:
			//	(e0 == '^' && e1 == '=') ||
			//	(e0 == '&' && e1 == '=') ||
			//	(e0 == '|' && e1 == '=') ||
			//	(e0 == '+' && e1 == '=') ||
			//	(e0 == '-' && e1 == '=') ||
			//	(e0 == '/' && e1 == '=') ||
			//	(e0 == '*' && e1 == '=') ||
			//	(e0 == '%' && e1 == '=') 
			//	){
			//	return 2;
			//}
			//1 char length objects:
			/*else*//*if ((*pElementName)){*//*if( (*pElementName)=='(' || (*pElementName)==')'
									  || (*pElementName)=='[' || (*pElementName)==']'
									  || (*pElementName)=='{' || (*pElementName)=='}'
									  || (*pElementName)==','
									  || (*pElementName)=='$'
									  || (*pElementName)=='*'
									  || (*pElementName)=='-'
									  || (*pElementName)=='/'
									  || (*pElementName)=='+'
									  || (*pElementName)=='='
									  || (*pElementName)=='\''
									  || (*pElementName)=='\"'
									  || (*pElementName)==';'
									  || (*pElementName)==':'
									  || (*pElementName)=='?'
									  || (*pElementName)==','

									  )*/
			return 1/*$*/;
			//}
		}
	}
	return 0;
}



int sauce_printfs(char *pBuf, int pBufSize, const char *pStr, ...){
	va_list vl;
	va_start(vl, pStr);
	int r = sauce_vprintfs(pBuf, pBufSize, pStr, vl);
	va_end(vl);
	return r;
}
int sauce_vprintfs(char *pBuf, int pBufSize, const char *pStr, va_list vl){
	int i = 0;
	int len = 0;
	int pos = 0;
	long long p = 0;
	for (i = 0; pStr[i]; i++){
		if (pStr[i] == '%'){
			len = 0;
#ifdef _WIN64
			p = (va_arg(vl, long long));
#else
			p = (va_arg(vl, long));
#endif
			if (pStr[i + 1] == 'x'){
				IntToHexStr(p, 0, 0, &len);
				if (pos + len <= pBufSize){
					IntToHexStr(p, 0, pBuf + pos, &len);
				}
				pos += len;
			}
			else if (pStr[i + 1] == 'd') {
				IntToStr(p, 0, &len, 0);
				if (pos + len <= pBufSize) {
					IntToStr(p, pBuf + pos, &len, 0);
				}
				pos += len;
			}
			else if (pStr[i + 1] == 'u') {
				IntToStr(p, 0, &len, 1);
				if (pos + len <= pBufSize) {
					IntToStr(p, pBuf + pos, &len, 1);
				}
				pos += len;
			}
			else if (pStr[i + 1] == 'f') {
				DoubleToStr(*((double*)&p), 0, &len,20);
				if (pos + len <= pBufSize) {
					DoubleToStr(*((double*)&p), pBuf + pos, &len,20);
				}
				pos += len;
			}
			else if (pStr[i + 1] == 's' || pStr[i + 1] == 'e'/*element name*/){
				if (p){
					if (pStr[i + 1] == 'e'){
						len = GetElementNameLength((char*)p);
					}
					else{
						len = sauce_strlen((char*)p);
					}
					if (pos + len <= pBufSize){
						int i2;
						for (i2 = 0; i2 < len; i2++){
							pBuf[pos++] = ((char*)p)[i2];
						}
					}
					else{
						pos += len;
					}
				}
			}
			else { // %% %?
				i++; //move past %
				pBuf[pos++] = pStr[i];
			}

			i++; //move past ?
		}
		else if(pos<pBufSize) {
			pBuf[pos++] = pStr[i];
		}
		else pos++;
	}
	if (pos < pBufSize){
		pBuf[pos] = 0;
	}
	else if(pBufSize) {
		pos = pBufSize - 1;
		pBuf[pos] = 0;
	}

	return pos; //return num of characters that was or could have been written
}


long long Pow10[18] = {
	1000000000000000000,
	100000000000000000,
	10000000000000000,
	1000000000000000,
	100000000000000,
	10000000000000,
	1000000000000,
	100000000000,
	10000000000,
	1000000000, 
	100000000,
	10000000, 
	1000000,
	100000,
	10000, 
	1000, 
	100,
	10
};       // for binairy -> decimal convertion
// same as itoa, but faster
void IntToStr(long long a, char *aStr, int *aLen, int isUnsigned) {
	long long l, d, s;

	l = 0;
	s = 0;
	if (a < 0 && !isUnsigned){
		if(aStr) aStr[l] = '-';
		l++;
		a *= -1;
	}

	int i;
	for (i = 0; i<18; i++)  {
		d = a / Pow10[i];
		if (d)    {
			s = 1;
			a -= d*Pow10[i];
		}
		if (s){
			if (aStr)
				aStr[l] = '0' + (char)d;
			l++;
		}
	}

	l++;
	if (aStr){
		aStr[l-1] = '0' + (char)a;
		aStr[l] = 0;
	}
	if (aLen) *aLen = (int)l;
}

void IntToHexStr(long long a, int digits/*0 for variable*/,char *aStr, int *aLen) {
	int curShift = 0;
	int isNeg = 0;

	unsigned long long n = (unsigned long long)a;
	do{
		curShift+=4;
		n >>= 4;
	} while( (curShift < digits || (!digits&&n)) );

	n = (unsigned long long)a;
	int l = 0;
	while (curShift){
		curShift -= 4;
		int digit = (n >> curShift)&(0xf);
		if (digit < 10) { if (aStr) aStr[l] = '0' + digit; }
		else { if (aStr) aStr[l] = 'A' + (digit - 10); }
		l++;
	}
	if(aStr) aStr[l] = 0;
	if(aLen) *aLen = l;
}

unsigned long long StrToLongLong(char *str, int *scanned){
	unsigned long long retN = 0;
	int nLen = 0;
	if (str[0] == '0' && str[1] == 'x'){
		//hex string
		nLen = 2;
		int ch;
		while (1){
			if (isNumberCharacter(str[nLen])){
				ch = (str[nLen] - '0');
			}
			else if (str[nLen] >= 'a' && str[nLen] <= 'f') {
				ch = (str[nLen] - 'a') + 10;
			}
			else if (str[nLen] >= 'A' && str[nLen] <= 'F'){
				ch = (str[nLen] - 'A') + 10;
			}
			else break;
			if (retN > (unsigned long long)0x0fffffffffffffff){
				if (scanned) *scanned = 0;
				return (long long)retN;
			}
			retN <<= 4;
			retN += ch;
			nLen++;
		}
	}
	else{
		while (isNumberCharacter(str[nLen])){
			if (retN > (unsigned long long)(0xffffffffffffffff/10)){
				if(scanned) *scanned = 0;
				return (long long)retN;
			}
			retN *= 10;
			retN += (str[nLen] - '0');
			nLen++;
		}
	}
	if (scanned) *scanned = nLen;
	return retN;
}
double StrToDouble(char *str, int *scanned){

	double retN = 0.0;
	double retDenom = 1.0;
	int exp = 0;

	int nLen = 0;
	double neg = 1.0;
	if (*str == '-') { neg = -1.0; nLen++; }
	while (isNumberCharacter(str[nLen])){
		retN *= 10.0;
		retN += (double)(str[nLen] - '0');
		nLen++;
	}
	if (str[nLen] == '.'){
		nLen++;
		while (isNumberCharacter(str[nLen])){
			retN *= 10.0;
			retN += (double)(str[nLen] - '0');
			retDenom *= 10.0;
			nLen++;
		}
	}
	if (str[nLen] == 'e' || str[nLen] == 'E'){
		nLen++;
		if (str[nLen] == '-'){
			nLen++;
			while (isNumberCharacter(str[nLen])){
				exp *= 10;
				exp += (int)(str[nLen] - '0');
				nLen++;
			}
			while (exp >= 0) {
				retDenom *= 10;
				exp--;
			}
			//retDenom *= pow(10, exp);
		}
		else {
			if (str[nLen] == '+') nLen++;
			while (isNumberCharacter(str[nLen])){
				exp *= 10;
				exp += (int)(str[nLen] - '0');
				nLen++;
			}
			while (exp >= 0) {
				retN *= 10;
				exp--;
			}
			//retN *= pow(10, exp);
		}
	}
	if (str[nLen] == 'f') {
		nLen++;
	}

	if (scanned) *scanned = nLen;

	if (retDenom != 1.0)
		return (retN*neg) / retDenom;
	return retN*neg;
}

void DoubleToStr(double d, char *dStr, int *dLen, int maxlen) {
	if (d == 0.0 ){
		if (dStr) {
			*dStr = '0';
			*(dStr + 1) = 0;
		}
		if (dLen) {
			*dLen = 1;
		}
		return;
	}
	if (*((unsigned long long*)&d) == (unsigned long long) - 1/*nan of infinity*/) {
		if (dStr) {
			*dStr = 'n';
			*(dStr + 1) = 'a';
			*(dStr + 2) = 'n';
			*(dStr + 3) = 0;
		}
		if (dLen) {
			*dLen = 3;
		}
		return;
	}
	//unsigned long long m = (*((unsigned long long*)&d)&0xfffffffffffff)|0x10000000000000;
	//long long e = (long long)((*((unsigned long long*)&d)&0x7fffffffffffffff) >> (64 - 12)) - 1023 - 52/*for mantissa*/;
	//unsigned long long s = (*((unsigned long long*)&d))>> (63);
	short be10 = 0;
	while (d > 1.0e100) {
		d /= 1.0e100;
		be10 += 100;
	}
	while (d > 1.0e10) {
		d /= 1.0e10;
		be10 += 10;
	}
	while (d > 10.0) {
		d /= 10.0;
		be10 += 1;
	}
	while (d < 0.0) {
		d *= 10;
		be10 -= 1;
	}
	int len = 0;
	int digit = (int)d;
	d -= (double)digit;
	if (dStr) {
		*dStr = '0' + digit;
		dStr++;
	}
	len++;
	if (d != 0.0) {
		if (dStr) {
			*dStr = '.';
			dStr++;
		}
		len++;
		while (d != 0.0 && len<maxlen) {
			d *= 10;
			digit = (int)d;
			d -= (double)digit;
			if (dStr) {
				*dStr = '0' + digit;
				dStr++;
			}
			len++;
		}
	}
	if (dLen) {
		*dLen = len;
	}
}

//returns length of name or 0 if not found
int GotoFilename(char **p_fullfilename) {
	char *namestart = *p_fullfilename;
	//if (*namestart == '\"') {
	//	namestart++;
	//}
	char *nameitr = namestart;
	while ((*nameitr) && (*nameitr) != '\"'/*with in #include"file.ext"*/) {
		if ((*nameitr) == '\\' || (*nameitr) == '/') {
			namestart = nameitr + 1;
		}
		nameitr++;
	}
	if (!isTextCharacter(*namestart) && ((*namestart) != '_')) {
		return 0;
	}

	int namelen = (int)(nameitr - namestart);
	*p_fullfilename = namestart;
	return namelen;
}
//returns length of extension or 0 if not found
int GotoFilenameExt(char **p_filename) {
	int filenamelen = GotoFilename(p_filename);
	char *nitr = *p_filename;
	while (*nitr && nitr[0] != '.') {
		nitr++;
	}
	int extlen = 0;
	if (nitr[0]=='.') {
		nitr++;
		*p_filename = nitr;
		while (*nitr && nitr[0] != '\"') {
			extlen++;
			nitr++;
		}
	}
	return extlen;
}
