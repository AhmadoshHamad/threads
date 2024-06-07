#include "all.h"

#define MAXSIZE 100 // max size of the line
#define THRESHOLD  MAXSIZE/2 //threshold

int lineNumber ;
int numOfAddEmp = 0;
int maxNumOfEmp ;
int randomSeed=0 ;
int openID;
void checking(int, int);
struct produced_medicine {
   
    bool notMissingPills;
    bool colorAndSIze;
    bool expiryDate;
    int pid;
    //int check; // for testing 1 is good 0 bad

};

struct produced_medicine m[MAXSIZE];

struct inspection_medicine {
    struct produced_medicine medicine;
};

struct packaging_medicine {
    struct produced_medicine medicine;
};

struct inspection_medicine ins[MAXSIZE];
struct packaging_medicine pac[MAXSIZE];

int produce_index = 0;
int inspection_index = 0;
int packaging_index = 0;

pthread_cond_t cond_produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_inspection = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_packaging = PTHREAD_COND_INITIALIZER;

pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ins_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pac_mutex = PTHREAD_MUTEX_INITIALIZER;

void *produce(void *arg) {
    int speed = rand() % 4 + 1;
   //printf("start producing..*PILLS* %d\n",lineNumber);
    while (1) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&cond_produce, &m_mutex);

        for (int i = 0; i < speed; i++) {
            m[produce_index % MAXSIZE].pid = randomSeed;
            m[produce_index % MAXSIZE].colorAndSIze=getRandom(minChanceColorSize,maxChanceColorSize,randomSeed);
            m[produce_index % MAXSIZE].expiryDate=getRandom(minChanceExpiryDate,maxChanceExpiryDate,randomSeed);
            m[produce_index % MAXSIZE].notMissingPills = getRandom(minChanceNumberOfPills,maxChanceNumberOfPills,randomSeed); 
          // printf("Adding new medicine %d\n",lineNumber);
            produce_index++;
            randomSeed++;
        }
        
        pthread_mutex_unlock(&m_mutex);
        sleep(2);
        pthread_cond_signal(&cond_inspection);
    }
    return NULL;
}

void *inspection(void *arg) {
    int line = *((int *)arg); // Dereference the pointer to get the integer value

  // printf("start inspection...*PILLS* %d\n",lineNumber);
    while (1) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&cond_inspection, &m_mutex);

        if (m[inspection_index % MAXSIZE].expiryDate == true && m[inspection_index % MAXSIZE].colorAndSIze == true && m[inspection_index % MAXSIZE].notMissingPills == true) {
            pthread_mutex_lock(&ins_mutex);
            ins[inspection_index % MAXSIZE].medicine = m[inspection_index % MAXSIZE];
            inspection_index++;
          printf("inspect new medicine and it is good *PILLS* \n");
            checking(produce_index,inspection_index);
            pthread_mutex_unlock(&ins_mutex);
            sendingSignal(openID, SIGRTMIN, line);
            pthread_cond_signal(&cond_packaging);
        } else {
           inspection_index++;
            sendingSignal(openID, SIGRTMIN+2, line);
      //      printf("inspect new medicine and it is bad *PILLS*\n");
           checking(produce_index,inspection_index);
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

    //printf("start packaging...*PILLS* %d\n",lineNumber);
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
        sleep(2);
        sendingSignal(openID, SIGRTMIN+3, line);
        pthread_cond_signal(&cond_inspection);
        pthread_cond_signal(&cond_produce);
    }
    return NULL;
}

void checking(int p  , int i){
    
        /* code */
      //  sleep(1);
        printf("\r *PILLS*-->number of produceing=%d --->number of inspection=%d --->number of packaging=%d\n",produce_index,inspection_index,packaging_index);
         printf("*PILLS*---->new Emp  %d THRESHOLD =%d <---\n",numOfAddEmp,p-i);
        if(numOfAddEmp != maxNumOfEmp){ // max number  of emp can i add is 2 
            if (p - i == THRESHOLD){//THRESHOLD
                printf("*PILLS*---->add new thread <---\n");
                pthread_t new_thread;
                
                pthread_create(&new_thread, NULL, inspection, NULL);
                numOfAddEmp ++;
            }
        }
    
    
}

int main(int argc, char *argv[]) {
    lineNumber = atoi(argv[1]);
    openID = atoi(argv[2]);
    int *lineNumberThread = (int*) malloc(sizeof(int)); // Allocate memory for the integer
    *lineNumberThread = lineNumber+5;
    maxNumOfEmp=readFromFile("maxNumOfAddEmp=");
    printf("%d num of Emp \n",numEmp);
    pthread_t prod_thread,check_thread;
    pthread_create(&prod_thread, NULL, produce, NULL);
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