#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

void* serverfunc(void* name){
	sleep(1);
	char* pname = (char*)name;
	int file = open(pname, O_RDWR);
	char filename[50];
	read(file, filename, sizeof(filename));
	cout << "Server:" << endl;
	cout << "Received filename: " << filename << endl;
	int txtf = open(filename, O_RDONLY);
	if (txtf < 0){
		cout << "File not found" << endl << endl;
		char extra = NULL;
		write(file, &extra, 1);
		pthread_exit(0);
	}
	char buffer[5120];
	ssize_t bytesRead = read(txtf, buffer, sizeof(buffer));
	cout << "Contents read from file: ";
	for (int x = 0; x < bytesRead; x++){
		cout << buffer[x];
	}
	cout << "Sending file contents to client" << endl;
	write(file, buffer, sizeof(buffer));
	cout << "Waiting for client..." << endl << endl;
	int* x = new int;
	*x = bytesRead;
	pthread_exit((void*)x);
}

int main(){
	char input[50];
	while (strcmp(input, "exit") != 0){
		cout << "Enter file name: ";
		cin >> input;
		if (strcmp(input, "exit") == 0){
			break;
		}
		cout << endl;
		cout << "Got filename ";
		cout << input;
		cout << " from command line." << endl;
		cout << "Sending filename to server" << endl;
		char* myfifo = "\mypipe";
		mkfifo(myfifo, 0666);
		int fd = open(myfifo, O_RDWR);
		write(fd, input, sizeof(input));
		pthread_t tid;
		if (pthread_create(&tid, NULL, &serverfunc, myfifo) != 0){
			perror("Thread not created");
			break;
		}
		cout << "Thread created with ID: " << tid << endl;
		cout << "Waiting for server to respond..." << endl << endl;
		int* size;
		if(pthread_join(tid, (void**)&size) != 0){
			perror("Thread not joined.");
			break;
		}
		char bfr[5120];
		read(fd, bfr, sizeof(bfr));
		sleep(1);
		cout << "Back to client" << endl;
		if (bfr[0] != NULL){
			cout << "Contents of file: ";
			for (int x = 0; x < *size; x++){
				cout << bfr[x];
			}
		}
		else {
			cout << "File not found";
		}
		cout << endl;
		close(fd);
		cout << "----------------------------------------------" << endl << endl;
	}
	return 0;
}
