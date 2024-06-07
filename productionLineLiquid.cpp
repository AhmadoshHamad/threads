// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
#include "all.h"

#include "Liquid.h"

void *produce(void *arg) {
    int line = *((int *)arg); // Dereference the pointer to get the integer value
    int speed = rand() % 4 + 1;
    //printf("*LIQUID* start producing..%d\n",lineNumber);
    while (1) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&cond_produce, &m_mutex);

        for (int i = 0; i < speed; i++) {
            m[produce_index % MAXSIZE].pid = randomSeed;
            m[produce_index % MAXSIZE].level = getRandom(minLiquidLevelRange,maxLiquidLevelRange,randomSeed);
            m[produce_index % MAXSIZE].color=getRandom(minLiquidColor,maxLiquidColor,randomSeed);
            m[produce_index % MAXSIZE].sealed=getRandom(minChanceBottleSealed,maxChanceBottleSealed,randomSeed);
            m[produce_index % MAXSIZE].expiryDate=getRandom(minChanceExpiryDate,maxChanceExpiryDate,randomSeed);
            m[produce_index % MAXSIZE].label=getRandom(minChanceLabel,maxChanceLabel,randomSeed);


     //      printf("*LIQUID*Adding new medicine %d\n",lineNumber);
            produce_index++;
            randomSeed++;
        }
        
        pthread_mutex_unlock(&m_mutex);
        sendingSignal(openID, SIGRTMIN+6, line);
        sleep(2);
        pthread_cond_signal(&cond_inspection);
    }
    return NULL;
}
int *lineNumberThread = (int*) malloc(sizeof(int)); // Allocate memory for the integer
void *inspection(void *arg) {
    int line = *((int *)arg); // Dereference the pointer to get the integer value
        printf("%d", line);
   // printf("start inspection...*LIQUID*%d\n",lineNumber);
    while (1) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&cond_inspection, &m_mutex);

        if (m[inspection_index % MAXSIZE].level == true && m[inspection_index % MAXSIZE].label == true && m[inspection_index % MAXSIZE].color == true && m[inspection_index % MAXSIZE].expiryDate == true && m[inspection_index % MAXSIZE].sealed == true) {
            pthread_mutex_lock(&ins_mutex);
            ins[inspection_index % MAXSIZE].medicine = m[inspection_index % MAXSIZE];
            inspection_index++;
        //  printf("inspect new medicine and it is good *LIQUID*\n");
            // checking(produce_index,inspection_index);
            pthread_mutex_unlock(&ins_mutex);
            pthread_cond_signal(&cond_packaging);
            sendingSignal(openID, SIGRTMIN, line);
        } else {
           inspection_index++;
           parameter_ptr->bottleFail++;
           sendingSignal(openID, SIGRTMIN+2, line);
           printf("inspect new medicine and it is bad *LIQUID*\n");
        //    checking(produce_index,inspection_index);
        }
        pthread_mutex_unlock(&m_mutex);
        sleep(2);
        pthread_cond_signal(&cond_inspection);
        pthread_cond_signal(&cond_produce);
    }
    return NULL;
}

void *packaging(void *arg) {
    int line = *((int *)arg); // Dereference the pointer to get the integer value
    //printf("*LIQUID*start packaging...%d\n",lineNumber);
    while (1) {
        pthread_mutex_lock(&pac_mutex);
        pthread_cond_wait(&cond_packaging, &pac_mutex);
        pthread_mutex_lock(&ins_mutex);
        pac[packaging_index % MAXSIZE].medicine = ins[packaging_index % MAXSIZE].medicine;
        packaging_index++;
        //printf("packaging new medicine and it is good\n");
        pthread_mutex_unlock(&ins_mutex);
        pthread_mutex_unlock(&pac_mutex);
        sendingSignal(openID, SIGRTMIN+1, line);
        sendingSignal(openID, SIGRTMIN+3, line);
        parameter_ptr->success++;
        sleep(2);
        pthread_cond_signal(&cond_inspection);
        pthread_cond_signal(&cond_produce);
    }
    return NULL;
}
// void signal_catcher(int sig){
//     int pid = getpid();
//     if (pthread_cancel(thread) != 0) {
//             perror("Error cancelling thread");
//             exit(EXIT_FAILURE);
//     }
// }

void checking(int p  , int i){
    
    /* code */
    //  sleep(1);
    printf("\r *LIQUID*-->number of produceing=%d --->number of inspection=%d --->number of packaging=%d\n",produce_index,inspection_index,packaging_index);
        printf("*LOQUID*---->new Emp  %d THRESHOLD =%d <---\n",numOfAddEmp,p-i);
    if(numOfAddEmp != maxNumOfEmp){ // max number  of emp can i add is 2 
        if (p - i >= THRESHOLD){//THRESHOLD
            printf("*LIQUID*---->add new thread <---\n");
            numOfAddEmp ++;
        }
    }

}
int main(int argc, char *argv[]) {
    lineNumber = atoi(argv[1]);
    openID = atoi(argv[2]);
    maxNumOfEmp=readFromFile("maxNumOfAddEmp=");
    *lineNumberThread = lineNumber;
    pthread_t prod_thread;
    pthread_create(&prod_thread, NULL, produce, (void *)lineNumberThread);
//    pthread_create(&check_thread, NULL, checking, NULL);

    pthread_t ins_thread[numEmp];
    pthread_t pac_thread[numEmp];
    sleep(1);
    pthread_cond_signal(&cond_produce);
    sleep(1);
    
    for (long i = 0; i < numEmp; i++) {
        pthread_create(&ins_thread[i], NULL, inspection, (void *)lineNumberThread);
        pthread_create(&pac_thread[i], NULL, packaging, (void *)lineNumberThread);
    }
    // while(1){
    //     checking(produce_index,inspection_index);
    //     if (sigset(SIGUSR2, signal_catcher) == SIG_ERR ) {
    //         perror("Sigset can not set SIGUSR1");
    //         exit(SIGUSR2);
    //     }
    //     sleep(2);
    // }
    pthread_join(prod_thread, NULL);
    //pthread_join(check_thread, NULL);
    for (int i = 0; i < numEmp; i++) {
        pthread_join(ins_thread[i], NULL);
        pthread_join(pac_thread[i], NULL);
    }

    pthread_cond_destroy(&cond_produce);
    pthread_cond_destroy(&cond_inspection);
    pthread_cond_destroy(&cond_packaging);
    pthread_mutex_destroy(&m_mutex);
    pthread_mutex_destroy(&ins_mutex);
    pthread_mutex_destroy(&pac_mutex);

    return 0;
}