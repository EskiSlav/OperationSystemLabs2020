#include <stdio.h>
#include <pthread.h>

#define TEXT "Test text"

void * printLines(void * param);
int main(int argv, char * argc[])
{
    pthread_attr_t attr;
    pthread_t p1;

    pthread_attr_init(&attr);
    pthread_create(&p1, &attr, printLines, TEXT);

    // int status;
    // pthread_join(p1, &status);

    printf("%s\n", "Parent process:");
    // printf("Ret: %d | Status: %d\n", ret, status);
    // if(status == 0);
    printLines(TEXT);
    return 0;
    
}

void * printLines(void * param)
{
    for(int i = 0; i < 4; i++)
        printf("%s\n", param);
    pthread_exit(0);
}