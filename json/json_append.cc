#include "json.h"

jsonkeypair *jsonobj::AppendText(const char *key, const char *val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    unsigned long vlen = 0;
    while (val[vlen]) vlen++;
    return AppendText(key, val, vlen, alloc,free);
}
jsonkeypair *jsonobj::AppendText(const char *key, const char *val, const unsigned long len, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonstring* str = (jsonstring*)alloc(sizeof(jsonstring));
    if(!str) return 0;
    long err = str->SetString(val, len, alloc);
    if(err<0){ str->m_ftable->Delete(str,free); return 0; }
    jsonkeypair* kp = AppendObj(key, str, alloc, free); // will free str on failure
    return kp;
}
jsonkeypair *jsonobj::AppendObj(const char *key, _jsonobj* val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonkeypair* kp = (jsonkeypair*)alloc(sizeof(jsonkeypair)); if(!kp) return 0;
    long err;
    if((err=kp->Init(alloc, key, val))<0){ kp->Free(free)/*will free val as well*/; free(kp); return 0; }
    if( (err = addChild(kp,free,alloc))<0/*Resize failed*/ ){ kp->Free(free)/*will free val as well*/;free(kp); return 0; }
    return kp;
}
jsonkeypair *jsonobj::AppendNumber(const char *key, double num, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonnumber* number = (jsonnumber*)alloc(sizeof(jsonnumber));
    if(!number) return 0;
    number->num = num;
    jsonkeypair* kp = AppendObj(key, number, alloc, free); // will free number on failure
    return kp;
}
