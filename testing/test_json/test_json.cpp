// test_json.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../../json/json.h"
#include <stdio.h>
#include <stdlib.h>

#include "../../stream/stream.h"
#include "../../stream/text_stream.h"

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
        return 0;
    }
};

char jsontext[] = "{ \"name\":\"John Doe\", \"age\":30, \"isStudent\":false, \"courses\":[\"Math\",\"Science\",\"History\"], \"address\": { \"street\":\"123 Main St\", \"city\":\"Anytown\", \"zip\":\"12345\" } }";   

int main(){
    text_stream ts(jsontext);
    printstream ps;

    jsonobj* root = 0;
    int err = jsonobj_Create(( _jsonobj**)&root, malloc);
    if(err<0){
        printf("Failed to create json object\n");
        return -1;
    }
    err = root->Load(&ts, malloc, free);
    ps.Init(0);
    ps.PutBytes("Hello, Dude!\n", 14);
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
