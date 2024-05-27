#include "productionHeader.h"

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;  

void* inspect(void* arg){
    int i;
    while(1){
        pthread_mutex_lock(&mutex);
        while(1){
            i=0;
            if(bottles_ptr->bottle[i].pid==getpid()){
                break;
            }
        }
        if(bottles_ptr->bottle[i].color && bottles_ptr->bottle[i].expiryDate && bottles_ptr->bottle[i].label
        && bottles_ptr->bottle[i].level && bottles_ptr->bottle[i].sealed){
            printf("Medicine with index %d is good\n", i);
        }
        else{
            printf("Medicine with index %d is bad\n", i);
        }
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char * argv[]){
    int index = atoi(argv[1]);
    pthread_t thread[numberOfEmployees];
    for(int i=0; i<numberOfEmployees; i++){
        if(pthread_create(&thread[i], NULL, &inspect, (void*) &thread[i])!=0){
            return 0;
        }     
    }
    while(1){
        sem_wait(sem);
        bottles_ptr->bottle[index].pid=getpid();
        bottles_ptr->bottle[index].color=getRandom(minLiquidColor, maxLiquidColor);
        bottles_ptr->bottle[index].expiryDate=getRandom(minChanceExpiryDate, maxChanceExpiryDate);
        bottles_ptr->bottle[index].label=getRandom(minChanceLabel, maxChanceLabel);
        bottles_ptr->bottle[index].level=getRandom(minLiquidLevelRange, maxLiquidLevelRange);
        bottles_ptr->bottle[index].sealed=getRandom(minChanceBottleSealed, maxChanceBottleSealed);
        sem_post(sem);
        cout << "Medicine produced by " << getpid() << endl;
        sleep(5);   
    }
    return 0;
}