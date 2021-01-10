#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* from 1 to 4 */
int NUM_C_THREADS;
/* critical region */
int SUM = 0;

/* sheared control variables */
int request = 0, respond = 0;

/* client thread method */
void *client_func(void *params){
    int tid = (int)params;
    /* inf loop */
    for( ; ; ){
        while(respond != tid){
            request = tid;
        }
        /* critical region */
        int local = SUM;
        sleep(rand()%2);
        SUM = local + 1;

        /* print */
        printf("Thread: %d -> SUM = %d\n", tid, SUM);
        fflush(stdout);

        respond = 0;
    }   
}

/* server thread method */
void *server_func(void *params){
    /* inf loop */
    for( ; ; ){
        /* await request != 0 */
        while(request == 0);
        respond = request;
        /* await respond == 0 */
        while(respond != 0);
        request = 0;
    }
}

/*
* Compile: gcc -o with busy_wait_with_mutual_exclusion.c -pthread (dont use any optimizer, may cause problems)
* Run: ./with NUM_C_THREADS
*/
int main(int argc, char **argv){

    /* to identify errors */
    int er;

    /* number of client threads */
    NUM_C_THREADS = atoi(argv[1]);

    /* threads instantiation */
    pthread_t client_thread[NUM_C_THREADS];
    pthread_t server_thread;

    /* threads creation */
    er = pthread_create(&server_thread, NULL, server_func, NULL);
    if(er){
            printf("Error: create = %d\n", er);
            exit(-1);
    }
    for(int i = 0; i < NUM_C_THREADS; i++){
        er = pthread_create(&client_thread[i], NULL, client_func, (void *)(i+1));
        if(er){
            printf("Error: create = %d\n", er);
            exit(-1);
        }
    }

    pthread_exit(NULL);

    return 0;
}