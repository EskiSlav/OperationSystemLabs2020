#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


static int cleanup_pop_arg = 0;

void * printLines(void * param);
int main(int argv, char * argc[])
{
    pthread_t thread;

    char ** text = (char**)malloc(3*sizeof(char*));
    text[0] = "thread"; 
    text[1] = "first"; 
    text[2] = "Heh Ipsum";
    pthread_create(&thread, NULL, printLines, (void*)text);
   
    pthread_cancel(thread);
    printf("Child process canceled\n");
    sleep(1);
    return 0;
}

static void onCancel(void * arg)
{
    printf("Child process says that it was canceled by someone =(;\n");
}

void * printLines(void * text)
{
    pthread_cleanup_push(onCancel, NULL);
    int done = 0;
    int i = 0;
    while(!done)
    {
        
        pthread_testcancel();
        
        printf("%s\n", ((char**)text)[i]);
        i++;
        if(i == 3)
            done = 1;
        sleep(1);
    }
    pthread_cleanup_pop(cleanup_pop_arg);
}
