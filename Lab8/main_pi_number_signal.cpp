#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

int step;
int done = 0;
pthread_mutex_t mutex;


int get_sign(int num)
{
    if (num % 2 == 1)
        return -1;
    else
        return 1;
}


int next_check = 0;

void * get_pi(void * param)
{
    double * sum = (double*)malloc(sizeof(double));
    *sum = 0;
    int i = *(int*)param;
    
    while(1)
    {
        pthread_mutex_lock(&mutex);
        next_check += 10000;
        pthread_mutex_unlock(&mutex);
        for(; i < next_check; i += step)
        {
            double val = (double)get_sign(i - 1)/(double)(2 * i - 1);
            *sum += val;
        }
        if(!done)
            continue;
        else
        {    
            printf("Thread: %d | Sum: %.10f\n", *(int*)param, (*sum) * 4);
            pthread_exit((void*)sum);
        }   
    }
    
}

void sigint_handler(int sig)
{
    printf("\nGot Signal: %d\n", sig);
    if (sig == 2)
    {
        done = 1;
    }
}

int main(int argv, char * argc[])
{
    if (argv != 2)
        return 1;

    signal(SIGINT, sigint_handler);
    const int threads_number = atoi(argc[1]);
    step = threads_number;
    pthread_t threads[threads_number];
    pthread_mutex_init(&mutex, NULL);
    printf("The number of threads: %d\n", threads_number);

    for(int i = 0; i < threads_number; i++)
    {
        int * thread_number = (int*)malloc(sizeof(int));
        *thread_number = i+1;
        pthread_create(&(threads[i]), NULL, get_pi, (void*)(thread_number));
    }
    void * status;
    double sum;
    for(int i = 0; i < threads_number; i++)
    {
        pthread_join(threads[i], &status);
        sum += *((double*)status);
    }
    printf("Pi number: %.25f\n", sum*4);
}