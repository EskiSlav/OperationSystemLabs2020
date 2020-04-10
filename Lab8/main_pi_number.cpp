#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int step;

int get_sign(int num)
{
    if (num % 2 == 1)
        return -1;
    else
        return 1;
}

void * get_pi(void * param)
{
    double * sum = (double*)malloc(sizeof(double));
    *sum = 0;
    for(int i = *(int*)param; i < 1000000000; i += step)
    {
        double val = (double)get_sign(i - 1)/(double)(2 * i - 1);
        *sum += val;
    }
    printf("Thread: %d | Sum: %.10f\n", *(int*)param, (*sum) * 4);
    return (void*)sum;
}

int main(int argv, char * argc[])
{
    if (argv != 2)
        return 1;

    const int threads_number = atoi(argc[1]);
    step = threads_number;
    printf("The number of threads: %d\n", threads_number);
    pthread_t threads[threads_number];

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
    printf("%.30f\n", sum*4);
}