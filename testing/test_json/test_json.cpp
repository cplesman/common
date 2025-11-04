// test_json.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "../../json/json.h"
#include <stdio.h>
#include <stdlib.h>

#include "../../stream/stream.h"
#include "../../stream/text_stream.h"
#include <cassert>

void *memorytest[32768];
int numallocs = 0;
void* testalloc(unsigned long s) {
    void* ret = malloc(s);
    if(ret)
        memorytest[numallocs++] = ret;
    return ret;
}
void testfree(void *l) {
    int i = 0; bool found = false;
    for (i = 0; i < numallocs; i++) {
        if (memorytest[i] == l) {
            memorytest[i] = 0;
            found = true;
            break;
        }
    }
    assert(found);

    free(l);
}
int countallocations() {
    int count = 0;
    for (int i = 0; i < numallocs; i++) {
        if (memorytest[i]) count++;
    }
    return count;
}

class printstream : public stream {
public:
    long getBytes(char* p_buffer, long numbytes) override {
        if(numbytes) {
            for(long i=0;i<numbytes;i++) {
                p_buffer[i] = ' ';
            }
        }
        return numbytes;
    }
    long putBytes(const char* p_buffer, long numbytes) override {
        long bi = 0;
        char tmp[32];
        while(bi<numbytes  ){
            long i=0;
            while(bi<numbytes && i<31){
                tmp[i++] = p_buffer[bi++];
            }
            tmp[i] = 0;
            printf("%s", tmp);
        }
        return bi;
    }
};

char jsontext[] =
"{\r\n"
"  'name': 'Test Object',\r\n"
"  'value': 123.456,\r\n"
"  'active': true,\r\n"
"  'items': [\r\n"
"    'Item 0',{\r\n"
"       'name': 'Sub Object', 'value': 789, \r\n"
"       'name2': 'Sub Object2', 'value2': 789, \r\n"
"     },\r\n"
"    'Item 2',\r\n"
"    'Item 3',\r\n"
"    'Item 4'\r\n"
"  ]\r\n"
"}";

int main(){
    text_stream ts(jsontext);
    printstream ps;

    jsonobj* root = 0;
    int err = jsonobj_Create(( _jsonobj**)&root, testalloc);
    if(err<0){
        printf("Failed to create json object\n");
        return -1;
    }
    err = root->Load(&ts, testalloc, testfree);

    ps.Init(0);
    root->Send(&ps, 2);

    printf("object at key 'items' = \n");
    jsonkeypair* items = (*root)["items"];
    items->val->Send(&ps, 2);

    jsonobj_Delete(root,testfree);

#ifdef _WIN32
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif

    printf("num allocations left = %d\r\n", countallocations());
    printf("total that was allocated = %d\r\n", numallocs);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
