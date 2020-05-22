#include <sys/types.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/msg.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <list>

#define PROG_COUNT 3
#define LOOPS_FOR_SUBPROCESS 3
#define MSGSZ 2048

typedef struct message{
    long mtype;
    char mtext[MSGSZ];
} message;


int main()
{
    int key = ftok("/tmp/", 32);
    int msgid = msgget(key, 0666|IPC_CREAT);
    message msg;
    while(true)
    {
        if(msgrcv(msgid, &msg, MSGSZ, 1, 0) < 0)
            perror("msgrcv");

        printf("Message: %s", msg.mtext );
    }
}