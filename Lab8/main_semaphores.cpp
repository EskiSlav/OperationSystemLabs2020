#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <semaphore.h>
#include <time.h>
using std::queue;

queue<char*> q;
const int max_size = 5;
FILE * file;
int done = 0;

sem_t lock;
sem_t full;
sem_t empty;

void * produce(void * param)
{
    char * text;
    while(!done)
    {
        sem_wait(&empty);
        sem_wait(&lock);
        text = (char*)malloc(255 * sizeof(char));
        if (fgets(text, 256, file) != NULL)
        {
            q.push(text);
            printf("%s %d [!] Text has been added! \n\n", (char*)param, pthread_self());
        }
        else
        {
            done = 1;
        }
        sem_post(&lock);
        sem_post(&full);
        sleep(rand() % 4); // random sleep;
    }
    return NULL;
}

void * consume(void * param)
{
    while(!done || !q.empty()) // when both conditions is False exit the loop;
    {
        sem_wait(&full);
        sem_wait(&lock);
        if(!q.empty())
        {
            printf("%s: %d [!] Text: \n----------------------------------\n %s-------------------------------------\n",(char*)param , pthread_self(), q.front());
            q.pop();
        }
        sem_post(&lock);
        sem_post(&empty);
        sleep(rand() % 4); // random sleep;
    }
    return NULL;
}

int main(int argv, char * argc[])
{
    srand(time(0));

    sem_init(&lock, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty,0, max_size);
    file = fopen("lorem_ipsum.txt", "r");

    pthread_t cons1, cons2;
    pthread_t prod1, prod2;

    pthread_create(&prod1, NULL, produce, (void*)"producer1");
    pthread_create(&prod2, NULL, produce, (void*)"producer2");
    pthread_create(&cons1, NULL, consume, (void*)"consumer1");
    pthread_create(&cons2, NULL, consume, (void*)"consumer2");

    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);
    
    return 0;
}