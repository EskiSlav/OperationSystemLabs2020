#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define FILES 8

int * fd = new int[FILES];


void print_descriptors(int * fd, int print_offset=1)
{
    for(int i = 0; i < print_offset; i++) printf(" ");

    int opened_files = 0;
    for(int i = 0; i < FILES; i++)
    {
        struct stat buff;
        if (fstat(fd[i], &buff) == -1)
            continue;
        else
        {
            opened_files++;
            printf("%d ", fd[i]);
        }
    }
    printf("\n");
    for(int i = 0; i < print_offset; i++) printf(" ");
    printf("Total opened files:%d\n\n", opened_files);
}

int main() {
    // Array of file descriptors
    fd[0] = open("/dev/stdin", O_RDWR); 
    fd[1] = open("/dev/stdout", O_RDWR); 
    fd[2] = open("/dev/stderr", O_RDWR); 
    fd[3] = open("./files/dum.txt", O_RDWR); 
    fd[4] = open("./files/rob.txt", O_RDWR); 
    fd[5] = open("./files/ann.txt", O_RDWR); 
    fd[6] = open("./files/kate.txt", O_RDWR); 
    fd[7] = open("./files/file.txt", O_RDWR); 
    

    printf(" - PID:%d All opened descriptors:\n", getpid());
    print_descriptors(fd);

    printf(" - PID:%d Closing some descriptors ... \n", getpid());
    close(fd[2]);
    close(fd[3]);


    printf(" - PID:%d Show descriptors again:\n", getpid());
    print_descriptors(fd);

    printf(" - PID:%d Creating Child Process...\n", getpid());
    pid_t pID = fork();

    switch(pID)
    {
        case 0:
            // Child process section
            printf(" - - PID:%d Child process showing descriptors...\n", getpid());
            print_descriptors(fd, 5);  

            printf(" - - PID:%d Child process closing some descriptors...\n", getpid());
            close(fd[6]);
            close(fd[5]);

            printf(" - - PID:%d Show descriptors in child process after closing:\n", getpid());
            print_descriptors(fd, 5);

            printf(" - - PID:%d Child process EXITING...\n", getpid());
            break;

        case -1:
            // Error Section
            printf("Something bad happened when creating process\n");
            perror("fork");
            break;

        default:
            // Parent process section
            wait(&pID);
            printf(" - PID:%d Parent process waited for child and now prints descriptors again\n", getpid());
            print_descriptors(fd);
            printf(" - PID:%d Parent process EXITING...\n", getpid());
            break;
    }
    

}

