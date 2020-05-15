////////g++ -pthread -o Main Main.cpp -ldl -lrt

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <aio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/sem.h>
#include <dirent.h>
#include <string.h>

using namespace std;

union semun {
      int val;               
      struct semid_ds *buf;    
      unsigned short *array;                                  
      struct seminfo *__buf;  
};


struct MyFile {
	char path[30];
	char text[100];
	int size;
	int handle;
	struct aiocb aioStruct;
} fileToRead, fileToWrite;

void *dll;
void (*readFile)(struct MyFile*);
void (*writeFile)(struct MyFile*);
int semID;

void semwait(int index) {
	while(true){
		usleep(10000);
		if(semctl(semID, index, GETVAL, 0) == 1){
			semctl(semID, index, SETVAL, 0);
			break;
		}
	}
}

void *readThreadFunction(void *arg) {
	fileToRead.aioStruct.aio_offset = 0;
	fileToRead.aioStruct.aio_buf = fileToRead.text;
	fileToRead.aioStruct.aio_nbytes = 100;
	fileToRead.aioStruct.aio_reqprio = 0;
	fileToRead.aioStruct.aio_sigevent.sigev_notify = SIGEV_NONE;
	
	struct stat st;
	DIR *dir;
	struct dirent *ent;
	dir = opendir ("./files");
	while (true) {
		semwait(1);
		while (true) {
			if ((ent = readdir (dir)) != NULL) {
				if(ent->d_name[0] == '.')
					continue;
				break;
			} else {				
				semctl(semID, 2, SETVAL, 1);
				semctl(semID, 0, SETVAL, 1);
				closedir (dir);
				return NULL;
			}  
		}
		strcpy(fileToRead.path, "files/");	
		for (int i = 0; i < 20; i++)	fileToRead.path[i + 6] = ent->d_name[i];
		fileToRead.aioStruct.aio_fildes = open(fileToRead.path, O_RDONLY);
		readFile(&fileToRead);
		
		strcpy(fileToWrite.text, fileToRead.text);
		stat(fileToRead.path, &st);
		fileToWrite.aioStruct.aio_nbytes = st.st_size;			
		memset(fileToRead.path, 0, 30);
		memset(fileToRead.text, 0, 100);		
		semctl(semID, 0, SETVAL, 1);
	}
}

void *writeThreadFunction(void *arg) {
	fileToWrite.aioStruct.aio_offset = 0;
	fileToWrite.aioStruct.aio_fildes = open("filesTogether.txt", O_WRONLY | O_CREAT | O_APPEND |O_TRUNC);
	fileToWrite.aioStruct.aio_buf = fileToWrite.text;
	fileToWrite.aioStruct.aio_reqprio = 0;
	fileToWrite.aioStruct.aio_sigevent.sigev_notify = SIGEV_NONE;
	while (true) {
		semwait(0);
		if(semctl(semID, 2, GETVAL, 0) == 1){
			return NULL;
		}
		writeFile(&fileToWrite);
		memset(fileToWrite.path, 0, 100);
		semctl(semID, 1, SETVAL, 1);
	}
}

int main() {
	semID = semget(11111, 3, IPC_CREAT | 0666);
	semctl(semID, 1, SETVAL, 1);	
	
	dll = dlopen("/home/andrey/Projects/lw5/lw5_dll/myDLL.so",RTLD_NOW);
	readFile = (void(*)(struct MyFile*)) dlsym(dll,"myRead");
	writeFile = (void(*)(struct MyFile*)) dlsym(dll,"myWrite");	
	
	cout << "Run threads";
	pthread_t threads[2];
	pthread_create(&threads[0], NULL, readThreadFunction, NULL);
	pthread_create(&threads[1], NULL, writeThreadFunction, NULL);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	cout << "\nThreads are finished\n";

	semun arg;
	semctl(semID, 0, IPC_RMID, arg);
	semctl(semID, 1, IPC_RMID, arg);
	semctl(semID, 2, IPC_RMID, arg);
	dlclose(dll);
	return 0;
}
