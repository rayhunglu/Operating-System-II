#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define total_num_chair 5
#define customers 25

sem_t waitcust;
pthread_mutex_t accesslock;
sem_t waitbarber;
int num_chair;
void *barber();
void *customer(void *arg);
int sleeptime;

int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    pthread_mutex_init(&accesslock, NULL);
    pthread_t barber_thread;
    pthread_t customers_thread[customers];
    
    sem_init(&waitcust, 0, 0);
    sem_init(&waitbarber, 0, 0);
    
    num_chair=total_num_chair;
    
    int args[customers],i;
    for (i = 0; i < customers; i++)
        args[i] = i;
    
    pthread_create(&barber_thread, NULL, barber, NULL);
    for (i = 0; i < customers; i++)
        pthread_create(&customers_thread[i], NULL, customer, (void *)&args[i]);
    
    pthread_join(barber_thread, NULL);
    for (i = 0; i < customers; i++)
        pthread_join(customers_thread[i], NULL);
    
    return 0;
}
void *barber() {
    while (1) {
        printf("The barber is ready to cut hair.\n");
        sem_wait(&waitcust);
        pthread_mutex_lock(&accesslock);

        num_chair++;
        sem_post(&waitbarber);
        pthread_mutex_unlock(&accesslock);
        sleeptime=rand()%4+3;
        sleep(sleeptime);
    }
}

void *customer(void *arg) {
    int customerid = *((int *)arg);
    while (1) {
        pthread_mutex_lock(&accesslock);
        if (num_chair > 0) {
            num_chair--;
            printf("Customer %d is waiting.\n", customerid);
            
            sem_post(&waitcust);
            pthread_mutex_unlock(&accesslock);
            sem_wait(&waitbarber);
            
            printf("The barber is cutting #%d.\n", customerid);
            sleeptime=rand()%4+3;
            sleep(sleeptime);
            
        }
        else {
            printf("No chair, customer %d leaves.\n", customerid);
            pthread_mutex_unlock(&accesslock);
            sleeptime=rand()%4+3;
            sleep(sleeptime);
        }
    }
}
