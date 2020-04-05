#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include <unistd.h>

const int max_size = 10;
int locked = 0;
int full = 0;
// int empty = max_size;
char text[255] = "";
// Declaration of thread condition variable 
// pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
// declaring mutex 
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 


void checkIfFull()
{
    while(full == max_size)
    {
        // just empty loop
    }
}

void checkIfEmpty()
{
    while(full == 0)
    {
        // just an empty loop
    }
}

// void wait(int * S)
// {
//     (*S)--;
// }

// void signal(int * S)
// {
//     (*S)++;
// }

void lock()
{
    while(locked == 1)
    {
        // empty loop
    }
    locked = 1;
}
void unlock()
{
    locked = 0;
}

void * producer(void * q)
{
    char *** queue = q;
    FILE * file = fopen((*queue)[0], "r");
    while(1)
    {
        checkIfFull();
        lock();
        if (fgets(text, 255, file) == NULL)
        {
            continue;
        }
        else
        {
            for(int i = 0; i < max_size; i++)
            {
                if((*queue)[i] == NULL)
                {
                    (*queue)[i] = text;
                    break;
                }
            }
        }
        full++;
        unlock();
    }

}

void * consumer(void * q)
{
    char *** queue = q;
    while(1)
    {
        checkIfEmpty();
        lock();
        for(int i = 0; i < max_size; i++)
        {
            if(queue[i] != NULL)
            {
                printf("%s",(*queue)[i]);
                queue[i] = NULL;
                break;
            }
        }
        full--;
        unlock();
    }
}

int main(int argv, char * argc[])
{
    if (argv < 2)
    {
        printf("Error, were not passed any argument!\n");
        return 2;
    } 
    else if (argv > 2)
    {
        printf("Too many arguments were passed to the programm\n");
        return 2;
    }

    char ** queue = malloc(max_size * sizeof(char*));

    for(int i = 0; i < max_size; i++)
        queue[i] = NULL;

    pthread_t cons, prod;
    pthread_attr_t cons_attr, prod_attr;

    pthread_attr_init(&cons_attr);
    pthread_attr_init(&prod_attr);

    for(int i = 0; i < max_size; i++)
        queue[i] = argc[1];
    
    pthread_create(&prod, &prod_attr, producer, &queue);
    pthread_create(&cons, &cons_attr, consumer, &queue);

    pthread_join(cons, NULL);

    return 0;
}