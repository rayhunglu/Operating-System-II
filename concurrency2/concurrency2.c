//933293709
//Jui-Hung Lu
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define N       5
#define LEFT    (i-1+N)%N
#define RIGHT   (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
char *philosophers[] = {"ph1", "ph2", "ph3", "ph4", "ph5"};

sem_t mutex;
sem_t S[N];

void think(int i)
{
    int think_time;
    think_time = (rand() % 20 + 1);
    printf("philosopher %s is thinking.\n", philosophers[i]);
    sleep(think_time);
}

void take_forks(int i)
{
    sem_wait(&mutex);
    state[i]=HUNGRY;
    if(state[i]=HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        sem_post(&S[i]);

    }
    sem_post(&mutex);
    sem_wait(&S[i]);
}

void eat(int i)
{
    int eat_time;
    eat_time = (rand() % 7 + 2);
    
    if(state[i] == EATING)
    {
        printf("philosopher %s is eating, take fork %d and fork %d.\n", philosophers[i],i+1,RIGHT+1);
    }
    sleep(eat_time);
}

void put_forks(int i)
{
    sem_wait(&mutex);
    state[i] = THINKING;
    printf("philosopher %s put fork %d and fork %d.\n",philosophers[i],i+1,RIGHT+1);
    i=LEFT;
    if(state[LEFT]=HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        sem_post(&S[i]);

    }
    i=RIGHT;
    i=RIGHT;
    if(state[i]=HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        sem_post(&S[i]);

    }
    sem_post(&mutex);
}

void *philosopher(void *num)
{
    int *i = num;
    state[*i] = THINKING;
    printf("philosopher %5s is entering.......\n", philosophers[*i]);
    while (1)
    {
        think(*i);
        take_forks(*i);
        eat(*i);
        put_forks(*i);
    }
}

int main()
{
    srand(time(NULL));
    int i;
    pthread_t id[N];
    sem_init(&mutex, 0, 1);
    for (i = 0; i < N; i++){
        sem_init(&S[i], 0, 0);
    }
    int phiIdx[N] = {
        0,
        1,
        2,
        3,
        4
    };
    for(i = 0; i < N ; i++)
    {
            int ret;
            ret=pthread_create(&id[i],NULL,philosopher, &phiIdx[i]);
            if(ret!=0)
            {
                printf ("Create pthread error!/n");
                return 1;
            }
    }
    for (i = 0; i < N; i++){
        pthread_join(id[i],NULL);
    }
}
