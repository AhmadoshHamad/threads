#include "all.h"

#define LIQUID 0
#define PILLS 1  

struct medicin
{
    /* data */
    int id ;
    int type;
};

int main(int argc, char *argv[]) {

    int num_medicines = readFromFile("numberOfMedicines=");
    struct medicin medicines [num_medicines];
    // creat the medicin
    for (int i=0 ;i < num_medicines ; i++){
        medicines[i].id =i;
        medicines[i].type=rand()%2 ;   //LIQUID rand()%2;
        printf("p[%d]the tyep is %d \n",medicines[i].id,medicines[i].type);
        if (medicines[i].type==LIQUID){
            if(!fork()){
                //printf("majd\n");
                execlp("./productionLineLiquid", "productionLineLiquid", to_string(medicines[i].id).c_str(), nullptr);
            } 
        }else{
            // if(!fork()){
            //     //printf("majd\n");
            //     execlp("./productionLinePILLS", "productionLinePILLS", to_string(medicines[i].id).c_str(), nullptr);
            // } 
            printf("im PILLS\n"); //productionLinePILLS
            
        }
    }
    wait(NULL);
    wait(NULL);

}