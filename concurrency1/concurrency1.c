#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include "mt19937ar.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t stopproducer,stopconsumer;

int idx = 0;
int front = 0;
int end = 0;
int checkrdrand=0;

struct Data{
        int number;
        int wait_time;
};

struct Data buffer[32];
//get rdrand or mt19937 number
int randomminmax(unsigned int min, unsigned int max){
        unsigned int value;
        if(min > max){
                printf("IMPROPER RAND ARGS");
                value = 0;
                return value;
        }
        unsigned int range = (max - min)+1;

        char vendor[13];
        if(checkrdrand){
        //use rdrand
            unsigned int gen;
            unsigned char isOk;

            asm volatile("rdrand %0; setc %1"
                         : "=r"(gen), "=qm"(isOk));

            // 1 = success, 0 = underflow
            if (isOk)
                value= ((gen % max) + min);
            else
                value= 0;
        }
        else{
        //use mt19937
            init_genrand(time(NULL));
            // min to max
            //1 ~ 9
            int x = (int)genrand_int32();
            value= (abs(x) % (max - 1)) + min;
        }
        return value;
}
void producer_check_idx(){
    if(idx >= 32){
        printf("Producer is waiting!\n");
        pthread_cond_signal(&stopconsumer);
        pthread_cond_wait(&stopproducer,&lock);
    }
}
void consumer_check_idx(){
    if(idx < 1){
        printf("Consumer waiting\n");
        pthread_cond_signal(&stopproducer);
        pthread_cond_wait(&stopconsumer,&lock);
    }
}
void *producer(){
    int number = 0;
    int waittime = 0;

    while(1){
        pthread_mutex_lock(&lock);
        producer_check_idx();
        number=0;
        number=randomminmax(1,10);
        waittime = 0;
        waittime = randomminmax(2,9);
        if (end == 32) end = 0;
        buffer[end].number = number;
        buffer[end].wait_time = waittime;
        end++;
        idx++;
        printf("Producer:%d, %d\n", number, waittime);
        
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&stopconsumer);
        
        sleep(randomminmax(2, 6));

    }
    pthread_exit(0);
}

void *consumer(){

        while(1){
            pthread_mutex_lock(&lock);

            consumer_check_idx();

            if (front == 32) front = 0;
            idx--;
            struct Data *popout=&(buffer[front++]);

            printf("Consumer value1:%d  idx:%d\n", popout->number, idx);
            printf("Consumer value2:%d  idx:%d\n", popout->wait_time, idx);
            //consumer sleep
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&stopproducer);
            sleep(popout->wait_time);

        }
    pthread_exit(0);
}

int main(int argc, char **argv){
    int num_threads;
    if(argc <= 1){
        num_threads=1;
    }
    else{
        printf("multiple threads\n");
        num_threads = atoi(argv[1]);
    }
    srand(time(NULL));

//check cpu
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    eax = 0x01;
    __asm__ __volatile__(
                         "cpuid;"
                         : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                         : "a"(eax));
    
    checkrdrand = ecx & 0x40000000;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&stopconsumer, NULL);
    pthread_cond_init(&stopproducer, NULL);
    
    num_threads=num_threads*2;
    pthread_t threads[num_threads];
    
    
    int i;
    for (i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, consumer, NULL);
        i++;
        pthread_create(&threads[i], NULL, producer, NULL);
    }
    
    for (i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);
    
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&stopconsumer);
    pthread_cond_destroy(&stopproducer);
    
    return 0;
}
