#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<math.h>
#include<time.h>
#include<semaphore.h>

#define N 5
#define NS 10

pthread_mutex_t mutexID, mutexSD;

typedef struct singly_list{
    int data;
    struct singly_list *next;
}node;
int total=0;

void* Searcher(void* ihead)
{
    sleep(2);
    node *head = (node*)ihead;
    while(1){
        int i = rand()%NS+1; /*Generate random number*/
        
        pthread_mutex_lock(&mutexSD);
        node *current = head->next;
        while(current != NULL) {
            if(current->data != i && current->next==NULL){
                printf("Cannot search %d \n",i);
                break;
            }
            if(current->data == i){
                printf("Searching %d \n",i);
                break;
            }
            current = current->next;
        }
        pthread_mutex_unlock(&mutexSD);
        i = rand()%NS+1;
        sleep(i);
    }
}

void* Deleter(void* ihead)
{
    sleep(2);
    node *head = (node*)ihead;
    while (1)
    {
        int i = rand()%total; /*Generate random number*/
        
        pthread_mutex_lock(&mutexSD);
        pthread_mutex_lock(&mutexID);

        node *pre = head->next;
        node *current;
        if(pre){
            i--;
            current = pre->next;
            while(current && current->next && i!=0) {
                i--;
                pre = current;
                current = pre->next;
            }
        }
        sleep(1);
        if(current->next!=NULL){
            pre->next = current->next;
            i=current->data;
        }else{
//            pre->next=NULL;
            i=current->data;
        }

        printf("Delete %d\n",i);
        pthread_mutex_unlock(&mutexSD);
        pthread_mutex_unlock(&mutexID);
        
        i = rand()%NS+1;
        sleep(i);
    }
}

void* Inserter(void* ihead)
{
    sleep(1);
    node *head = (node*)ihead;
    while (1)
    {
        int i = rand()%NS+1; /*Generate random number*/
        
        pthread_mutex_lock(&mutexID);
        node *current = head;
        while(current->next != NULL){
            current = current->next;
        }
        
        current->next = malloc(sizeof(node));
        current->next->data = i;
        current->next->next = NULL;
        pthread_mutex_unlock(&mutexID);
        printf("Inserted %d\n",i);
        total++;

        i = rand()%NS+1;
        sleep(i);
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    
    node *head = malloc(sizeof(struct singly_list));
    head->data = 0;
    head->next = NULL;
    
    int i;
    pthread_t inserter[N], deleter[N],searcher[NS];
    
    pthread_mutex_init(&mutexID, NULL);
    pthread_mutex_init(&mutexSD, NULL);
    
    for (i = 0; i < N; i++)
    {
        pthread_create(&inserter[i], NULL, Inserter, head);
    }
    for (i = 0; i < N; i++)
    {
        pthread_create(&deleter[i], NULL, Deleter, head);
    }
    for(i = 0; i < NS; i++)
    {
        pthread_create(&searcher[i], NULL, Searcher, head);
    }
    
    for (i = 0; i < N; i++)
    {
        pthread_join(inserter[i], NULL);
    }
    for (i = 0; i < N; i++)
    {
        pthread_join(deleter[i], NULL);
    }
    for(i = 0; i < NS; i++)
        pthread_join(searcher[i], NULL);
}
