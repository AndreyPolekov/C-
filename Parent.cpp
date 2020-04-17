#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  } 
  return 0;
}

vector<pid_t> pids;

void createChildPricess() {
	pid_t pid = fork();
    switch(pid){
		case -1:
			printf("\nChild error\n");
			exit(0);
		case 0:	
			execv("/home/andrey/Projects/lw2/Child/Child", (char* const*) {NULL});
			exit(0);
		default:
			pids.push_back(pid);
			break;	
	}	
}

void deleteChildPricess() {
	if(pids.size()) {
		kill(*(pids.end() - 1), SIGKILL);
		pids.pop_back();
	}
}

int main() {	
	while(true) {		
		system("clear");
		if(pids.size()) {
			for(unsigned int i = 0; i < pids.size(); i++) {
				kill(pids[i], SIGUSR1);				
				usleep(600000);	
			}
		}	
		if (kbhit()) {
			switch (getchar()) {
			case '+':
				createChildPricess();
				break;
			case '-':
				deleteChildPricess();
				break;
			case 'q':
				while (pids.size()) {
					deleteChildPricess();
				}
				return 0;
			}
		}
	}
}
