#include "base64.h"

static const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Return true if c is a valid Base64 character (alphanumeric or '+' or '/')
static inline bool isBase64(char c) {
    if( c>'a' && c <='z')
        return true;
    if( c>='A' && c <='Z')
        return true;
    if( c>='0' && c <='9')
        return true;
    return c == '+' || c == '/';
}

int toBase64(char *output, const unsigned char *input, int inputLen) {
    int i = 0, j = 0;
    unsigned char array3[3];
    unsigned char array4[4];
    if(inputLen <=0){
        output[0] = 0;
        return BASE64_ERROR_INVALIDDATA;
    }

    while (inputLen--) {
        array3[i++] = *(input++);
        if (i == 3) {
            array4[0] = (array3[0] & 0xfc) >> 2;
            array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
            array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
            array4[3] = array3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                output[j++] = base64chars[array4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (int k = i; k < 3; k++) {
            array3[k] = '\0';
        }

        array4[0] = (array3[0] & 0xfc) >> 2;
        array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
        array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
        array4[3] = array3[2] & 0x3f;

        for (int k = 0; k < i + 1; k++) {
            output[j++] = base64chars[array4[k]];
        }

        while ((i++ < 3)) {
            output[j++] = '=';
        }
    }

    output[j] = '\0'; // Null-terminate the output string
    return j; // Return the length of the output string
}

int fromBase64(unsigned char *output, const char *input, int inputLen) {
    int i = 0, j = 0;
    unsigned char array4[4], array3[3];
    int in_len = inputLen;
    if(inputLen <=0){
        output[0] = 0;
        return BASE64_ERROR_INVALIDDATA;
    }

    while (in_len-- && (input[j] != '=') && isBase64(input[j])) {
        array4[i++] = input[j++];
        if (i ==4) {
            for (i = 0; i <4; i++) {
                if (array4[i] >= 'A' && array4[i] <= 'Z') array4[i] = array4[i] - 'A';
                else if (array4[i] >= 'a' && array4[i] <= 'z') array4[i] = array4[i] - 'a' + 26;
                else if (array4[i] >= '0' && array4[i] <= '9') array4[i] = array4[i] - '0' + 52;
                else if (array4[i] == '+') array4[i] = 62;
                else if (array4[i] == '/') array4[i] = 63;
            }

            array3[0] = (array4[0] << 2) + ((array4[1] & 0x30) >> 4);
            array3[1] = ((array4[1] & 0xf) << 4) + ((array4[2] & 0x3c) >> 2);
            array3[2] = ((array4[2] & 0x3) << 6) + array4[3];

            for (i = 0; i < 3; i++) {
                output[j - 4 + i] = array3[i];
            }
            i = 0;
        }
    }

    if(!isBase64(input[j]) && input[j] != '=' && input[j]!=0){
        //we did not reach end but found invalid character
        return BASE64_ERROR_INVALIDDATA;
    }

    if (i) {
        for (int k = i; k <4; k++) {
            array4[k] = 0;
        }

        for (int k = 0; k <4; k++) {
            if (array4[k] >= 'A' && array4[k] <= 'Z') array4[k] = array4[k] - 'A';
            else if (array4[k] >= 'a' && array4[k] <= 'z') array4[k] = array4[k] - 'a' + 26;
            else if (array4[k] >= '0' && array4[k] <= '9') array4[k] = array4[k] - '0' + 52;
            else if (array4[k] == '+') array4[k] = 62;  
            else if (array4[k] == '/') array4[k] = 63;
        }
        array3[0] = (array4[0] << 2) + ((array4[1] & 0x30) >> 4);
        array3[1] = ((array4[1] & 0xf) << 4) + ((array4[2] & 0x3c) >> 2);
        array3[2] = ((array4[2] & 0x3) << 6) + array4[3];
        for (int k = 0; k < i - 1; k++) {
            output[j - 4 + k] = array3[k];
        }
    }
    return j - 4 + i - 1; // Return the length of the output data
}
