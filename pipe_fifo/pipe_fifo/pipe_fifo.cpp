#include <iostream>
#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"



using namespace std;

#define ERR_EXIT(msg) \
	do \
	{ \
		perror(msg); \
		exit(EXIT_FAILURE); \
	} while (0) 

#define Pipe(pipex) \
	do \
	{ \
		if (pipe(pipex) < 0) \
		ERR_EXIT("pipe"); \
	} while (0)

#define Close(fds) \
	do \
	{ \
		if (close(fds) < 0) \
			ERR_EXIT("close"); \
	} while (0);


int main(int argc, char *argv[])
{

	pid_t pid;//进程 ID
	int pipe1[2], pipe2[2];

	//创建 pipe1
	cout << "create pipe1" << endl;
	Pipe(pipe1);

	//创建 pipe2
	cout << "create pipe2" << endl;
	Pipe(pipe2);


	pid = fork();//Fork 一个新的进程

	if (pid == 0) {
		//子进程
		cout << "child process close pipe1[1] and pipe2[0]" << endl;
		Close(pipe1[1])
		Close(pipe2[0]);

		fork();
		//我们在此处再次创建了一个子线程的子线程  
		// 用于和子线程竞争管道数据 
		// 看 当一个管道有两个读出端的时候  数据如何流动

		//char buffer_send[1024];
		//memset(buffer_send, 0, sizeof(buffer_send));
		//snprintf(buffer_send, sizeof(buffer_send), "hello this is child %d", i);
		//string message = buffer_send;
		////string message = "hello this is child \r\n";
		//write(pipe2[1], message.data(), message.length());
		
		char buffer[10240];
		cout << "recv from father [" << read(pipe1[0], buffer, sizeof(buffer)) << "] = " << buffer << endl;

		exit(0);
	}
	else {
		//父进程
		cout << "father process chose pipe1[0] and pipe2[1]" << endl;
		Close(pipe1[0]);
		Close(pipe2[1]);

		for (int i = 0;i<200;++i)
		{
			char buffer_send[1024];
			memset(buffer_send, 0, sizeof(buffer_send));
			snprintf(buffer_send, sizeof(buffer_send), "hello this is child %d\r\n", i);
			string message = buffer_send;
			//string message = "hello this is child \r\n";
			write(pipe1[1], message.data(), message.length());
		}
		
		char buffer[128];
		cout << "recv from child [" << read(pipe2[0], buffer, sizeof(buffer)) << "] = " << buffer << endl;

	}

	wait();


	return 0;
}