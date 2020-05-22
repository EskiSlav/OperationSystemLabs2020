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
#define PROCESSES 4

typedef struct message{
    long mtype;
    char mtext[MSGSZ];
} message;

sem_t * sem_progs;
char ** shared;
char name[] = "MAIN";
int msgid;

int get_sem_value(sem_t sem);
int create_shared_programm_list();
int shared_semaphores_init();
int create_message_queue();
// int send_message(char * text);
void receive_messages();
void* thread_work(void * param);
void* check_semaphore(void * param);

int main()
{
    printf("%15s: state\n%13s\n", "Process name", "---------------------------");
    printf("%15s: Starting\n", name);
    printf("%15s: PID: %d\n", name, getpid());
    
    pid_t pID = 1;
    std::list<pid_t> pids;
    char * sIdentifier;
    
    srand(time(0));
    create_shared_programm_list();
    shared_semaphores_init();
    // create_message_queue();


    for(int i = 0; i < PROCESSES && pID > 0; i++)
    {
        sleep(1);
        pID = fork();
        if(pID > 0)
            pids.push_back(pID);
    }
    
    if (pID == 0)
    {
        srand(time(0));
        for(int i = 0; i < LOOPS_FOR_SUBPROCESS; i++)
        {
            pthread_t thread1;
            pthread_create(&thread1, NULL, thread_work, NULL);
            pthread_join(thread1, NULL);
        }
        
    }
    else if (pID < 0)
    {
        printf("Failed to fork\n");
        _exit(1);
        // Throw exception
    }
    else
    {
        // Code only executed by parent process
        for(auto iter : pids)
        wait(&iter);
        // receive_messages();
    }
    // Code executed by both parent and child.
    // printf("%s\n", sIdentifier);
    // printf("PID: %d\n", getpid());


    return 0;
}

int get_sem_value(sem_t sem)
{
    int value;
    sem_getvalue(&sem_progs[2], &value);
    return value;
}

// int create_message_queue()
// {
//     int key = ftok("/tmp/", 32);
//     msgid = msgget(key, 0666|IPC_CREAT);
//     printf("%15s: Created message queue:\n", name);
//     printf("%15s Queue id = %d\n", "-", msgid);
//     printf("%15s Key = %d\n", "-", key);

//     // if (msgctl(msgid, IPC_RMID, NULL) == -1) {
// 	// 	fprintf(stderr, "Message queue could not be deleted.\n");
// 	// 	exit(EXIT_FAILURE);
// 	// }
//     // printf("%15s Message queue was clened.\n", "-");

//     return 1;
// }

int create_shared_programm_list()
{
    int key = ftok("/tmp/", 8);
    int shmid = shmget(key, 1024, 0666|IPC_CREAT);
    if(shmid < 0)
        return -1;
    shared = (char**) shmat(shmid, (void*)0, 0);
    shared = new char*[PROG_COUNT];
    for(int i = 0; i < PROG_COUNT; i++)
        shared[i] = new char[20];
    strcpy(shared[0], "../exe/threading");
    strcpy(shared[1], "../exe/processes");
    strcpy(shared[2], "../exe/ferry");

    printf("%15s: Created shared programm list:\n", name);
    printf("%15s Key = %d\n", "-", key);
    for(int i = 0; i < PROG_COUNT; i++)
        printf("%15s %s\n", "-", shared[i]);

    return 1;
}

int shared_semaphores_init()
{
    int key = ftok("/tmp/", 97);
    int shmid = shmget(key, 1024, 0666|IPC_CREAT);
    if(shmid < 0)
        return -1;

    sem_progs = (sem_t*) shmat(shmid, (void*)0, 0);
    for(int i = 0; i < PROG_COUNT; i++)
        sem_init(&sem_progs[i],0,1);
    
    printf("%15s: Created semaphores for all programms\n", name);
    printf("%15s Key = %d\n", "-", key);
    for(int i = 0; i < PROG_COUNT; i++)
        printf("%15s Semaphore %d ('%15s'). Value:%d Address:%p\n", "-", i, shared[i], get_sem_value(sem_progs[i]), &sem_progs[i]);

    return 1;
}

char* itoa(int i)
{
    char * buffer = new char[10];
    sprintf(buffer, "%d", i);
    return buffer;
}

// int send_message(char * text)
// {  
//     char part[] = "Executing: ";
//     char * pid = itoa(getpid());
//     int bufsize;
//     char * newtext = new char[strlen(text) + strlen(part) + strlen(" PID: ") + strlen(pid) + 1];
//     strcpy(newtext, part);
//     strcat(newtext, text);
//     strcat(newtext, " PID: ");
//     strcat(newtext, itoa(getpid()));
//     printf("NewText: %s\n", newtext);
//     message msg;
//     msg.mtype = 1;
//     strcpy(msg.mtext, newtext);
//     bufsize = strlen(newtext) + 10;
//     printf("%15s: PID: %d: Sending message '%s' to the MAIN process...\n", "CHILD PROCESS", getpid(), newtext);
//     printf("%15s: Message size: %d\n", " ", bufsize);
//     if(msgsnd(msgid, &msg, bufsize, IPC_NOWAIT) < 0)
//     {
//         printf ("%d, %d, %s, %d\n", msgid, msg.mtype, msg.mtext, bufsize);
//         perror("msgsnd");
//     }
//     return 1;
// }

// void receive_messages()
// {
//     message msg;
//     for(int i = 0; i < 9; i++)
//     {
//         if(msgrcv(msgid, &msg, MSGSZ, 1, 0) < 0)
//         {
//             printf ("%d, %d, %s\n", msgid, msg.mtype, msg.mtext);
//             perror("msgsnd");
//         }
//         if(strcmp(msg.mtext, "Executing: ") > 0)
//             printf("%15s: Receied message: %s\n", name, msg.mtext);

//     }
// }

void* thread_work(void * param)
{

    int i = rand() % PROG_COUNT;
    message msg;
    
    printf("%15s: PID: %d: Choose for execution '%s'\n",
        "CHILD PROCESS", getpid(), shared[i]);
    printf("%15s: PID: %d: Sending message to the main process...\n", 
        "CHILD PROCESS", getpid());
    // while(get_sem_value(sem_progs[i]) == 0)
    //     sleep(1);
    printf("Semaphores pointers: %p | %p | %p\n", &sem_progs[0],&sem_progs[1],&sem_progs[2]);
    printf("Semaphores values  : %d | %d | %d\n", get_sem_value(sem_progs[0]),
                                                  get_sem_value(sem_progs[1]),
                                                  get_sem_value(sem_progs[2]));
    sem_wait(&(sem_progs[i]));
    perror("sem_wait");
    // send_message(shared[i]);
    printf("%15s: PID: %d Executing command:'%s'\n", "CHILD PROCESS", getpid(), shared[i]);
    system(shared[i]);
    sem_post(&sem_progs[i]);

    return NULL;
}