#include<stdio.h>
#include<semaphore.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <dirent.h> 
#include<stdlib.h>
#include<sys/wait.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

int losuj(int min, int max)
{
	return (rand()%(max-min+1)+min);
}

struct buf
{
	long msg;
	int num;
};

void pg(int msg_id, int n)
{
	struct buf b;
	for(int i = 0; i<n; i++)
	{
		b.num = losuj(1,1000);
		b.msg = (long) losuj(1,10);
		
		if(msgsnd(msg_id, &b, sizeof(int), 0) == -1)
		{
			printf("ERROR");
			exit(1);
		}
		
		printf("[%d] Typ: %ld  Dane: %d\n", i+1, b.msg, b.num);	
	}
}

void po(int msg_id, int kom_zak, int n)
{
	struct buf b;
	for(int i = 0; i<n; i++)
	{
		if(msgrcv(msg_id, &b, sizeof(int), kom_zak, MSG_EXCEPT) == -1)
		{
			printf("ERROR");
			exit(1);
		}
		
		printf("[%d] Typ: %ld  Dane: %d\n", i+1, b.msg, b.num);	
	}
	msgctl(msg_id, IPC_RMID, NULL);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	key_t klucz;
	int liczba_kom;
	int kom_zak;
	int msg_id;
	kom_zak = atoi(argv[1]);
	liczba_kom = atoi(argv[2]);
	msg_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	
	if(msg_id == -1)
	{
		printf("ERROR");
		exit(1);
	}
	
	switch(fork())
	{
		case 0:
		printf("PG(%d):", getpid());
		pg(msg_id, liczba_kom);
		exit(0);
		
		case -1:
		printf("ERROR");
		exit (1);
		
		default:
		switch(fork())
		{
			case 0:
			printf("PO(%d):", getpid());
			po(msg_id, kom_zak, liczba_kom);
			exit(0);
			
			case -1:
			printf("ERROR");
			exit (1);
			
			default:
			wait(NULL);
			exit(0);
		}
	}
	
	return 0;
}
