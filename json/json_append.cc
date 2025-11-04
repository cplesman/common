#include "json.h"

long jsonobj::AppendText(const char *key, const char *val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    unsigned long vlen = 0;
    while (val[vlen]) vlen++;
    return AppendText(key, val, vlen, alloc,free);
}
long jsonobj::AppendText(const char *key, const char *val, const unsigned long len, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonstring* str;
    long err = jsonstring_Create((_jsonobj**)&str, alloc);
    if(err<0) { str->m_ftable->Delete(str,free); return 0; }
    err = str->SetString(val, len, alloc);
    if(err<0){ str->m_ftable->Delete(str,free); return 0; }
    return AppendObj(key, str, alloc, free); // will free str on failure
}
long jsonobj::AppendObj(const char *key, _jsonobj* val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonkeypair* kp = (jsonkeypair*)alloc(sizeof(jsonkeypair)); if(!kp) return 0;
    long err;
    if((err=kp->Init(alloc, key, val))<0){ kp->Free(free)/*will free val as well*/; free(kp); return 0; }
    if( (err = addChild(kp,free,alloc))<0/*Resize failed*/ ){ kp->Free(free)/*will free val as well*/;free(kp); }
    return err;
}
long jsonobj::AppendNumber(const char *key, double num, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonnumber* number;
    long err = jsonnumber_Create((_jsonobj**)&number, alloc);
    if(err<0) { number->m_ftable->Delete(number,free); return 0; }
    number->num = num;
    return AppendObj(key, number, alloc, free); // will free number on failure
}
long jsonobj::AppendBoolean(const char *key, bool p_b, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonboolean* boolean;
    long err = jsonboolean_Create((_jsonobj**)&boolean, alloc);
    if(err<0) { boolean->m_ftable->Delete(boolean,free); return 0; }
    boolean->b = p_b;
    return AppendObj(key, boolean, alloc, free); // will free boolean on failure
}

long long jsonarray::AppendText(const char *val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    unsigned long vlen = 0;
    while (val[vlen]) vlen++;
    return AppendText(val, vlen, alloc,free);
}
long long jsonarray::AppendText(const char *val, const unsigned long len, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonstring* str;
    long err = jsonstring_Create((_jsonobj**)&str, alloc);
    if(err<0) { str->m_ftable->Delete(str,free); return 0; }
    err = str->SetString(val, len, alloc);
    if(err<0){ str->m_ftable->Delete(str,free); return 0; }
    return AppendObj(str, alloc, free); // will free str on failure
}
long long jsonarray::AppendObj(_jsonobj* val, void*(*alloc)(unsigned long), void(*free)(void*)) {
    if (m_size >= m_totalsize) {
        int err = Resize(m_totalsize ? m_totalsize * 2 : 4, free, alloc);
        if (err < 0) return err;
    }
    m_data[m_size] = val;
    m_size++;
    return (m_size-1);
}
long long jsonarray::AppendNumber(double num, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonnumber* number;
    long err = jsonnumber_Create((_jsonobj**)&number, alloc);
    if(err<0) { number->m_ftable->Delete(number,free); return 0; }
    number->num = num;
    return AppendObj(number, alloc, free); // will free number on failure
}
long long jsonarray::AppendBoolean(bool p_b, void*(*alloc)(unsigned long), void(*free)(void*)) {
    jsonboolean* boolean;
    long err = jsonboolean_Create((_jsonobj**)&boolean, alloc);
    if(err<0) { boolean->m_ftable->Delete(boolean,free); return 0; }
    boolean->b = p_b;
    return AppendObj(boolean, alloc, free); // will free boolean on failure
}
