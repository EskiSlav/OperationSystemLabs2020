#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

struct thread_info 
{
    int thread_number;
    char * name;
    pthread_t * thread;
};

std::queue<thread_info*> on_board;
std::queue<thread_info*> to_cancel;

sem_t busy_places, boarding, disembarkation;

int paratroopers_on_board = 0;
int marines_on_board = 0;
int max_places;
int handled_soldiers = 0;

void trip()
{
    printf("\n");
    for(int i = 0; i < 3; i++)
    {
        printf("%16s\n", "Ferry in trip!");
        sleep(1);
    }
    printf("\n");

    // Allow troops leave the ferry
    sem_post(&disembarkation);
}

void get_on_board(thread_info * info)
{
    sem_wait(&boarding);
    if(info->name == "Marine")
    {
        if((marines_on_board == max_places/2 && paratroopers_on_board != 0) || (paratroopers_on_board > max_places/2))
        {
            printf("%12s %d canceled ", info->name, info->thread_number);
            to_cancel.push(info);
        } 
        else
        {
            printf("%12s %d boarded  ", info->name, info->thread_number);
            on_board.push(info);
            marines_on_board++;
        }
    }
    else if (info->name == "Paratrooper")
    {
        if((paratroopers_on_board == max_places/2 && marines_on_board != 0) || (marines_on_board > max_places/2))
        {
            printf("%12s %d canceled ", info->name, info->thread_number);
            to_cancel.push(info);
        }        
        else
        {
            printf("%12s %d boarded  ", info->name, info->thread_number);
            on_board.push(info);
            paratroopers_on_board++;
        }
    }

    printf("< Current Time: %d \n", time(0));
    handled_soldiers++;
    sem_post(&boarding);
}

void get_out_of_board(thread_info * info)
{
    sem_wait(&disembarkation);
    printf("%12s %d disembarked. < Current time: %d\n", info->name, info->thread_number, time(0));
    sleep(1);
    sem_post(&disembarkation);
}

void clear_ferry()
{
    while(on_board.size() != 0)
    {
        // printf("Waiting clear\n");
        pthread_join(*(on_board.front()->thread), NULL);
        on_board.pop();
    }
}

void cancel_threads()
{
    while(to_cancel.size() != 0)
    {
        pthread_cancel(*(to_cancel.front()->thread));
        to_cancel.pop();
    }
}


void * thread_work(void * param)
{
    thread_info * info = (thread_info*)param;
    if (info->name == "Paratrooper")
        sleep(rand() % 3 + 1);
    else
        sleep(rand() % 3 + 2);
        
    get_on_board(info);
    get_out_of_board(info);
    pthread_exit(NULL);
}

int main(int argv, char * argc[])
{
    std::cout << "Enter N: ";
    int n;
    std::cin >> n;
    if(std::cin.fail())
    {
        std::cout << "Exiting ...\n";
        return 1;
    }

    srand(time(0));
    max_places = n * 2;
    printf("\n%16s\n", "INFORMATION");
    printf("%16s: %d\n", "Available places", max_places);
    printf("%16s: %d\n", "Paratroopers", max_places);
    printf("%16s: %d\n", "Marins", max_places);
    printf("%20s\n", "------------");    
    
    // Semaphores
    sem_init(&boarding, 0, 1);
    sem_init(&disembarkation, 0, 0);

    // Soldiers
    pthread_t * marine = new pthread_t[max_places];
    pthread_t * paratrooper = new pthread_t[max_places];

    int counter = 1;
    thread_info * info;
    for(int i = 0; i < max_places; i++)
    {
        // Creating marine
        info = new thread_info;
        info->thread_number = counter;
        info->thread = &marine[i];
        info->name = "Marine";
        pthread_create(&marine[i], NULL, thread_work, (void*)info);

        // Creating paratrooper
        info = new thread_info;
        info->thread_number = counter;
        info->thread = &paratrooper[i];
        info->name = "Paratrooper";
        pthread_create(&paratrooper[i], NULL, thread_work, ((void*)info));
        counter++;
    }

    // wait till all soldiers is canceled or boarded
    while(handled_soldiers != max_places*2)
    {
        sleep(1);
        continue;
    }

    // remove soldiers that could not board ferry
    printf("\nCleaning all threads troops that are not in ferry\n");
    cancel_threads(); 
    
    trip();

    // clear ferry on another side
    printf("Eject all troops on coast\n\n");
    clear_ferry();
    
    return 0;
}