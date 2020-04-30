#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <sys/sem.h>
#include <ncurses.h>

using namespace std;

union semun {
      int val;               
      struct semid_ds *buf;    
      unsigned short *array;                                  
      struct seminfo *__buf;  
};
pid_t childPid;

void childMain(int descriptors[]) {	
	int semID = semget(11111, 2, 0666);
	char string[100];
	while(true) {
		while(true){
			usleep(100000);
			if(semctl(semID, 2, GETVAL, 0) == 1){
				close(descriptors[0]);
				close(descriptors[1]);
				exit(0);
			}
			if(semctl(semID, 0, GETVAL, 0) == 1){
				printw("\nclient get 1 (sem[0])");
				refresh();
				semctl(semID, 0, SETVAL, 0);
				break;
			}
		}
		read(descriptors[0], string, 100);
		printw("\n%s", string);
		printw("\nclient set 1 (sem[1])");
		refresh();
		semctl(semID, 1, SETVAL, 1);
	}	
}

void createChildPricess(int descriptors[]) {
	pid_t childPid = fork();
    switch(childPid){
		case -1:
			printw("\nCreate child error\n");
			refresh();
			exit(0);
		case 0:
			childMain(descriptors);
	}	
}

int main() {	
	initscr();
	system("clear");
	int semID = semget(11111, 3, IPC_CREAT | 0666);
	int descriptors[2];
	if(pipe(descriptors) < 0){
		printw("Create pipe error.");
		refresh();
		return 0;
	}
	createChildPricess(descriptors);
	while(true) {	
		printw("Enter string:\n");
		refresh();
		char string[100];
		getstr(string);
		if (strcmp(string, "q") == 0) {	
			semun arg;					
			close(descriptors[0]);
			close(descriptors[1]);
			semctl(semID, 2, SETVAL, 1);
			semctl(semID, 0, IPC_RMID, arg);
			semctl(semID, 1, IPC_RMID, arg);
			semctl(semID, 2, IPC_RMID, arg);
			endwin();
			return 0;
		}	
		printw("\nserver set 1 (sem[0])");
		refresh();
		semctl(semID, 0, SETVAL, 1);		
		write(descriptors[1], string, 100);
		while(true){
			usleep(100000);
			if(semctl(semID, 1, GETVAL, 0) == 1){
				printw("\nserver get 1 (sem[1])");
				refresh();
				semctl(semID, 1, SETVAL, 0);
				break;
			}
		}
		printw("\n\n");
		refresh();
	}	
}
