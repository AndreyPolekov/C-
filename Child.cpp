#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;

bool output;

void handler(int sigNum) {
	signal(SIGUSR1, handler);
	output = true;
}

void destroy(int sigNum) {
	exit(0);
}

int main() {   
	signal(SIGUSR1, handler);
	signal(SIGKILL, destroy);
	
	ostringstream oss;
	oss << getpid();
	string pidStr = oss.str();
	while (true) {
		if(output) {
			for (unsigned int i = 0; i < pidStr.size(); i++) {
				printf("%c\n", pidStr[i]);
				printf("\033[1A");
				for (unsigned int j = 0; j <= i; j++) {
					printf("\033[1C");
				}
				usleep(100000);
			}
			for (unsigned int j = 0; j < pidStr.size(); j++) {
				printf("\033[1D");
			}
			printf("\n");	
			output = false;
		}
	}
    return 0;
}
