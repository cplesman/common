#ifdef __cplusplus
extern "C"{
#endif
int sauce_strlen(const char *pString);
int sauce_strlen_max(const char *pString, int max);
//sauce_strcpy
// returns pDest
char *sauce_strcpy(char *pDest, const char *pSource);
char *sauce_strncpy(char *pDest, const char *pSource, int pLen);
char *sauce_strcat(char *pDest, char *pSource);
char *sauce_strncat(char *pDest, char *pSource, int pLen);
//sauce_strcmp
// return difference of first characters that doesn't match
int sauce_strcmp(const char *p1, const char *p2);
int sauce_strncmp(const char *p1, const char *p2, int pLen);

char *sauce_tolower(char *d, char *s);
char *sauce_tolower_n(char *d, char *s, int n);
char* sauce_toupper(char *d, char *s);

#include <stdarg.h>
//pass 0 to pBUfSize to get number of characters needed
int sauce_vprintfs(char *pBuf, int pBufSize, const char *pStr, va_list vl);
int sauce_printfs(char *pBuf, int pBufSize, const char *pString, ...);

unsigned long long StrToLongLong(char *str, int *scanned);
double StrToDouble(char *str, int *scanned);
void DoubleToStr(double d, char *dStr, int *dLen, int maxlen/*must be atleast 5*/);
void IntToHexStr(long long a, int digits/*0 for variable*/, char *aStr, int *aLen);
void IntToStr(long long a, char *aStr, int *aLen, int isUnsigned);

int GotoFilename(char **p_fullfilename);
int GotoFilenameExt(char **p_filename);

int isIntegerText(char *str);  // only integer
int isFloatText(char *str, int pCheckDouble); //float and double if checking
int isNumberText(char *str);	//includes double, float, and integer

int GetElementNameLength(char *pElementName);

#define isWhiteCharacter(pCh)	((pCh) == '\n' || (pCh) == '\t' || (pCh) == '\r' || (pCh) == ' ')
#define isTextCharacter(pCh)	(((pCh) >= 'a' && (pCh) <= 'z') || ((pCh) >= 'A'&&(pCh) <= 'Z'))
#define isNumberCharacter(pCh)  ((pCh) >= '0' && (pCh) <= '9')
#ifdef __cplusplus
}
#endif
