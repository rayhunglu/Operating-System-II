#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
//Based on section 4.5 http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf

sem_t smokersem[3];
sem_t pushersem[3];
pthread_mutex_t agentlock;
pthread_mutex_t putlock;
char *agent_item[3] = {"matches & paper", "paper & tobacco", "tobacco & matches"};
int num_ingred[3] = {0, 0, 0},sleeptime;
void *agent(void *arg);
void *pusher(void *arg);
void *smoker(void *arg);

int main(int argc, char *arvg[]) {
    srand(time(NULL));
    
    int value[3]={0,1,2};
    pthread_t agentthr[3];
    pthread_t pusherthr[3];
    pthread_t smokerthr[3];
    
    int i;
    for (i = 0; i < 3; i++) {
        sem_init(&smokersem[i], 0, 0);
        sem_init(&pushersem[i], 0, 0);
    }
    
    pthread_mutex_init(&agentlock, NULL);
    pthread_mutex_init(&putlock, NULL);
    
    for (i = 0; i < 3; i++) {
        pthread_create(&agentthr[i], NULL, agent, &value[i]);
        pthread_create(&pusherthr[i], NULL, pusher, &value[i]);
        pthread_create(&smokerthr[i], NULL, smoker, &value[i]);
    }
    
    for (i = 0; i < 3; i++) {
        pthread_join(agentthr[i], NULL);
        pthread_join(pusherthr[i], NULL);
        pthread_join(smokerthr[i], NULL);
    }
    
    return 0;
}
void *agent(void *arg) {
    int agentid = *(int *)arg;
    
    while (1) {
        sleeptime=rand()%2+3;
        sleep(sleeptime);
        pthread_mutex_lock(&agentlock);
        
        sem_post(&pushersem[agentid]);
        sem_post(&pushersem[(agentid + 1) % 3]);
        
        printf("Agent %d provides items %s.\n", agentid, agent_item[agentid]);
    }
}
void *pusher(void *arg) {
    int pusherid = *(int *)arg;
    
    while (1) {
        sem_wait(&pushersem[pusherid]);
        pthread_mutex_lock(&putlock);
        
        if (num_ingred[(pusherid + 1) % 3]) {
            num_ingred[(pusherid + 1) % 3] -= 1;
            sem_post(&smokersem[pusherid]);
        }
        else if (num_ingred[(pusherid + 2) % 3]) {
            num_ingred[(pusherid + 2) % 3] -= 1;
            sem_post(&smokersem[(pusherid + 2) % 3]);
        }
        else {
            num_ingred[pusherid] += 1;
        }
        
        pthread_mutex_unlock(&putlock);
    }
}
void *smoker(void *arg) {
    int smokerid = *(int *)arg;
    
    while (1) {
        printf("Smoker %d is waiting for %s.\n", smokerid, agent_item[smokerid]);
        
        sem_wait(&smokersem[smokerid]);
        
        printf("Smoker %d is making a cigarette.\n", smokerid);
        sleeptime=rand()%2+3;
        sleep(sleeptime);
        pthread_mutex_unlock(&agentlock);
        
        printf("Smoker %d is smoking.\n", smokerid);
        sleeptime=rand()%2+3;
        sleep(sleeptime);
        
    }
}
