// test_http.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../../http.h"
#include "../../stream/text_stream.h"

#include <stdio.h>

char example_http_req[] = "\
GET /search?q=test HTTP/2\n\
Host: www.bing.com\n\
User-Agent: curl / 7.54.0\n\
Accept: */*\n\
\n\
";

class console_stream : public stream {
public:
	long getBytes(char *buf, long num) {
		long i = 0;
		while (i < num) {
			buf[i++] = ' '; //dummy bytes
		}
		return i;
	}
	long putBytes(const char *buf, long num) {
		long i = 0,i2=0;
		char tmp[32];
		tmp[31] = 0;
		while (i < num) {
			if( (buf[i] >= 32 && buf[i] <= 126) ||buf[i]=='\n'||buf[i]=='\r') {
				tmp[i2] = buf[i];
			}
			else {
				tmp[i2] = ' ';
			}
			i2++;
			i++;
			if (i2 == 31) {
				i2 = 0;
				printf(tmp);
			}
		}
		if (i2) {
			tmp[i2] = 0;
			printf(tmp);
		}
		return i;
	}
};

int main() {
	text_stream httpstream(&example_http_req[0]);
	console_stream con;
	http_header testheader;
	testheader.Load(&httpstream);
	testheader.headers.Send(&con, 1);
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
