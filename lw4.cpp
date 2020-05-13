//g++ -pthread -o lw4 lw4.cpp

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#include <sstream>
#include <pthread.h>
#include <iostream>
#include <sys/sem.h>

using namespace std;

pthread_mutex_t mutex;
int semID = semget(11111, 1, IPC_CREAT | 0666);
int threadsCuont = 0;
bool	deleteThread = false,
		createThread = false;

union semun {
      int val;               
      struct semid_ds *buf;    
      unsigned short *array;                                  
      struct seminfo *__buf;  
};
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
void semwait(int index) {
	while(true){
		usleep(10000);
		if(semctl(semID, index, GETVAL, 0) == 1){
			semctl(semID, index, SETVAL, 0);
			break;
		}
	}
}

void *myThreadFunction(void *arg) {
	usleep(10000);
	ostringstream oss;
	oss << pthread_self();
	string threadStr = oss.str();
	int threadIndex = threadsCuont++;
	while (1) {
		pthread_mutex_lock(&mutex);

		if (threadIndex == 0) {
			system("clear");
			semctl(semID, 0, SETVAL, 1);	
			usleep(1000);
			semwait(0);
			if (createThread) {
				createThread = false;
				pthread_t myThread;
				pthread_create(&myThread, NULL, myThreadFunction, NULL);	
			}
		}
		if (deleteThread && threadIndex == threadsCuont - 1) {
			threadsCuont--;
			deleteThread = false;
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}

		for (unsigned int i = 0; i < threadStr.size(); i++) {
			printf("%c\n", threadStr[i]);
			printf("\033[1A");
			for (unsigned int j = 0; j <= i; j++) {
				printf("\033[1C");
			}
			usleep(200000);
		}
		for (unsigned int j = 0; j < threadStr.size(); j++) {
			printf("\033[1D");
		}
		printf("\n");

		pthread_mutex_unlock(&mutex);
		usleep(10000);
	}
	return NULL;
}

int main() {
	system("clear");
	pthread_mutex_init(&mutex, NULL);
	while (1) {
		if (threadsCuont)
			semwait(0);

		if (kbhit()) {
			switch (getchar()) {
			case '+':
				if (threadsCuont == 0) {
					pthread_t myThread;
					pthread_create(&myThread, NULL, myThreadFunction, NULL);	
					continue;
				}
				else
					if (threadsCuont)
						createThread = true;
				break;
			case '-':
				deleteThread = true;
				break;
			case 'q':
				semun arg;
				semctl(semID, 0, IPC_RMID, arg);
				pthread_mutex_destroy(&mutex);
				return 0;
			}
		}
		semctl(semID, 0, SETVAL, 1);	
		usleep(1000);
	}
}



///////////////////////////////////////////			sem + thread
////g++ -pthread -o lw4 lw4.cpp

//#include <unistd.h>
//#include <stdio.h>
//#include <signal.h>
//#include <vector>
//#include <stdlib.h>
//#include <termios.h>
//#include <fcntl.h>

//#include <sstream>
//#include <pthread.h>
//#include <iostream>
//#include <sys/sem.h>

//using namespace std;

//pthread_mutex_t mutex;
//int semID = semget(11111, 2, IPC_CREAT | 0666);

//void semwait(int index) {
	//while(true){
		//usleep(10000);
		//if(semctl(semID, index, GETVAL, 0) == 1){
			//semctl(semID, index, SETVAL, 0);
			//break;
		//}
	//}
//}

//void *thread_func(void *arg) {
	//pthread_mutex_lock(&mutex);
	
	////for (int i = 0; i < 10; i++){
		////usleep(300000);
		////cout << " "<<i;
	////}
	//pthread_t thread1;/////////////////
	
	//ostringstream oss;
	//oss << pthread_self();
	//string pidStr = oss.str();
	//for (unsigned int i = 0; i < pidStr.size(); i++) {
		//printf("%c\n", pidStr[i]);
		//printf("\033[1A");
		//for (unsigned int j = 0; j <= i; j++) {
			//printf("\033[1C");
		//}
		//usleep(200000);
	//}
	//for (unsigned int j = 0; j < pidStr.size(); j++) {
		//printf("\033[1D");
	//}
	//printf("\n");
		
		//cout << "\n111 set";
	//semctl(semID, 0, SETVAL, 1);	
	
	//cout << "\n111 wait";
	//usleep(1000);
	//semwait(0);
		//cout << "\n111 end wait";
		
		
		
	//pthread_mutex_unlock(&mutex);	
	
	
	//return NULL;
//}

//int main() {
	//pthread_mutex_init(&mutex, NULL);
	//pthread_t thread1, thread2;
	
	
	//pthread_create(&thread1, NULL, thread_func, NULL);
	////pthread_create(&thread2, NULL, thread_func, NULL);
	////sleep(5);
	
	//cout << "\n wait\n";
	//semwait(0);
	
	//cout << "\n char:\n";
	//getchar();	
	
	//cout << "\n set\n";
	//semctl(semID, 0, SETVAL, 1);	
	//usleep(1000);
	//cout << "\n -------------------\n";
	//pthread_mutex_destroy(&mutex);
	//return 0;	
//}


