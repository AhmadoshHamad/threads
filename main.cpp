#include "all.h"
#include <unistd.h>
#define LIQUID 1
#define PILLS 0  

struct medicin
{
    /* data */
    int id ;
    int type;
};
void signal_catcher(int the_sig){
    printf("\nThe parent has received SIGINT .\n");
    int fd = open(fifo_lead, O_RDONLY);
    if (fd == -1) {
        perror("Error opening FIFO");
        return;
    }

    int received_pid;
    ssize_t bytesRead = read(fd, &received_pid, sizeof(int));
    if (bytesRead == sizeof(int)) {
        printf("Read from FIFO: PID = %d\n", received_pid);
    } else {
        perror("Error reading from FIFO");
    }
    int i=0;
    while(1){
        int pid = pids[getRandomValue(0,9,i)];
        if(pid != received_pid){
            kill(pid, SIGUSR2);
            break;
        }
        else{
            i++;
        }
        
    }
    close(fd);
} 
int pid;
void handle_alarm(int sig) {
printf("Ended because of signal\n");
  end(pid);
}
int main(int argc, char *argv[]) {
    parameter_ptr->success =0;
    parameter_ptr->pillFail =0;
    parameter_ptr->bottleFail=0;
    struct medicin medicines[numberOfProductionLines];
    int numberOfProductionLinesForPills = 0;
    int numberOfProductionLinesForLiquid = 0;

    int pipe_pills[2], pipe_liquid[2];
    pipe(pipe_pills);
    pipe(pipe_liquid);

    pid = fork();

    if (pid != 0) {
        for (int i = 0; i < numberOfProductionLines; i++) {
            medicines[i].id = i;
            medicines[i].type = getRandomValue(0,2, i); 
            if(numberOfProductionLinesForLiquid ==5){
                medicines[i].type = 0; 
            }
            if(numberOfProductionLinesForPills ==5){
                medicines[i].type = 1; 
            }          
            printf("p[%d]the type is %d \n", medicines[i].id, medicines[i].type);
            if (medicines[i].type == LIQUID) {
                numberOfProductionLinesForLiquid++;  // Increment count for liquid
                pids[i] = fork();
                if (pids[i] == 0) {
                    execlp("./productionLineLiquid", "productionLineLiquid", to_string(numberOfProductionLinesForLiquid).c_str(),to_string(pid).c_str(), nullptr);
                    exit(1);
                } 
            } else {
                numberOfProductionLinesForPills++; // Increment count for pills
                pids[i] = fork();
                if (pids[i] == 0) {
                    execlp("./productionLinePILLS", "productionLinePILLS",to_string(numberOfProductionLinesForPills).c_str(),to_string(pid).c_str(), nullptr);
                    exit(1);
                }             
            }
        }
        close(pipe_liquid[0]);
        write(pipe_liquid[1], &numberOfProductionLinesForLiquid, sizeof(int)); // Write count
        close(pipe_liquid[1]);
        close(pipe_pills[0]);
        write(pipe_pills[1], &numberOfProductionLinesForPills, sizeof(int)); // Write count
        close(pipe_pills[1]);
    }
    if (pid == 0) {
        close(pipe_pills[1]);
        close(pipe_liquid[1]);

        int pillsCount, liquidCount;

        // Read the counts from pipes
        read(pipe_pills[0], &pillsCount, sizeof(int));
        read(pipe_liquid[0], &liquidCount, sizeof(int));
 
        close(pipe_pills[0]);
        close(pipe_liquid[0]);

        execlp("./openGL", "openGL", to_string(liquidCount).c_str(), to_string(pillsCount).c_str(), nullptr);
        exit(1);
    }

    // if ( sigset(SIGUSR1, signal_catcher) == SIG_ERR ) {
    //     perror("Sigset can not set SIGUSR1");
    //     exit(SIGUSR1);
    // }

    // while (1) {
    //     pause(); // Wait for signals
    // }
    if (signal(SIGALRM, handle_alarm) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
  
    alarm(runningTime*60);
    while (1) {
        if (parameter_ptr->success >= numberOfMedicinesProduced || 
            parameter_ptr->bottleFail >= numberOfFailedBottledMedicine || 
            parameter_ptr->pillFail >= numberOfFailedPillMedicine) {
            printf("Ended because of parameters\n");
            end(pid);
        }
        sleep(2); // Sleep for 2 seconds before checking the condition again
    }
      
    wait(NULL);
    wait(NULL);
}
