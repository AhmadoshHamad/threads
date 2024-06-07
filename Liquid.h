#include <pthread.h>

#define MAXSIZE 40 // max size of the line
#define THRESHOLD  MAXSIZE/2 //threshold

int lineNumber ;

int numOfAddEmp = 0;
int maxNumOfEmp ;
int randomSeed =0 ;
int openID;

void checking(int  , int );
struct produced_medicine {
    bool level;
    bool color;
    bool sealed;
    bool label;
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