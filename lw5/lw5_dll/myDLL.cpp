//g++ -shared -o myDLL.so myDLL.cpp -lrt

#include <iostream>
#include <aio.h>
#include <errno.h>

using namespace std;

extern "C" {
	struct MyFile {
		char path[30];
		char text[100];
		int size;
		int handle;
		struct aiocb aioStruct;
	};
	
	void myRead(struct MyFile* file) {
		aio_read(&(file->aioStruct));
		while(aio_error(&(file->aioStruct)) == EINPROGRESS);
		cout << "\n" << file->text;
	}
	void myWrite(struct MyFile* file) {
		aio_write(&(file->aioStruct));
		while(aio_error(&(file->aioStruct)) == EINPROGRESS);
		file->aioStruct.aio_offset += file->size;
	}
}
