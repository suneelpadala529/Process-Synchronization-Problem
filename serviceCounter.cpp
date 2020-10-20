#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <queue>
#include <pthread.h>
#include <cstdlib>

#define MAX 3

using namespace std;

int n_counter=0;
int n_customers=0;

pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

sem_t limit;

void wait(sem_t &S)
{
    int value=0;
    sem_getvalue(&S,&value);
    if(value==0)
    {
        if(n_customers-2>MAX)
        {
            cout<<"[Queue full.. So exiting]"<<endl;
            pthread_mutex_lock(&mutex2);
            n_customers--;
            pthread_mutex_unlock(&mutex2);
            pthread_exit(0);
        }
        cout<<"Inside waiting queue"<<endl;
    }
    else if(value==1)
        cout<<"[One counter open, Sending wake up call]"<<endl;
    else
        cout<<"[Both counters closed, sending wake up call]"<<endl;
    sem_wait(&S);
}

void signal(sem_t &S)
{
    sem_post(&S);
    int value=0;
    sem_getvalue(&S,&value);
    if(value==2&&n_customers==1)
    {
        cout<<"[Counters are closed]"<<endl;
    }
    else if(value==1&&n_customers==2)
        cout<<"[One counter closed]"<<endl;
    else if(value==1)
        ;

}



void* thread(void* args)
{

    int t= *(int*) args;
    pthread_mutex_lock(&mutex2);
    n_customers++;                      //to change value of no fo threads(n_customers) acquire mutex lock
    pthread_mutex_unlock(&mutex2);
    cout<<"\nCustomer "<<pthread_self()<<" arrived "<<endl;
    wait(limit);
    cout<<"\n-----Servicing thread "<<pthread_self()<<" started-----"<<endl;
    sleep(t);                                                                   //critical section: sleep for time= waiting time
    cout<<"\n-----Servicing thread "<<pthread_self()<<" completed-----"<<endl;
    signal(limit);
    pthread_mutex_lock(&mutex2);
    n_customers--;                      //to change value of no fo threads(n_customers) acquire mutex lock
    pthread_mutex_unlock(&mutex2);

}


int main()
{
    sem_init(&limit,0,2);

    srand(time(NULL));
    int no_of_customers=10;


    pthread_t tid[no_of_customers];
    for(int i=0;i<no_of_customers;i++)
    {
        int *waiting_time;
        int * arrivaltime;
        waiting_time= (int*)malloc(sizeof(int));
        arrivaltime= (int*)malloc(sizeof(int));
        *waiting_time= rand()%5+1;
        *arrivaltime= rand()%5+1;
        //sleep for sometime and create the thread so as to simulate threads forming one by one.
        sleep(*arrivaltime);
        pthread_create(&tid[i],NULL,thread,(void*)waiting_time);

    }
    for(int i=0;i<no_of_customers;i++)
        pthread_join(tid[i],NULL);

    return 0;
}
