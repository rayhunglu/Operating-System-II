#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#define THREADS 10

pthread_t threads[3];
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int shared = 0;

void* use_resource(void* temp)
{
    int threadnum = (int*) temp;
    int i;
    while(1) {
        i=rand()%3+1;
        pthread_mutex_lock(&mutex1);
        shared++;
        printf("Process %d is running in %d second!\n", threadnum,i);
        if (shared == 3)
            printf("Full!\n");
        else
            pthread_mutex_unlock(&mutex1);
        
        sleep(i);

        pthread_mutex_lock(&mutex2);
        shared--;
        if (shared == 0)
        {
            printf("Clear!\n");
            pthread_mutex_unlock(&mutex1);
        }
        pthread_mutex_unlock(&mutex2);
        i=rand()%3+1;
        sleep(i);
    }
}

int main()
{
    srand(time(NULL));
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    int i;
    for(i=0; i<THREADS; i++)
        pthread_create(&threads[i], NULL, use_resource,i);
    
    for(i=0; i<THREADS; i++)
        pthread_join(threads[i], NULL);
    
    return 0;
}
