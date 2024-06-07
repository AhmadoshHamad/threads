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
int main(int argc, char *argv[]) {
    int num_medicines = readFromFile("numberOfMedicines=");
    struct medicin medicines[num_medicines];
    int numberOfProductionLinesForPills = 0;
    int numberOfProductionLinesForLiquid = 0;

    int pipe_pills[2], pipe_liquid[2];
    pipe(pipe_pills);
    pipe(pipe_liquid);

    int pid = fork();

    if (pid != 0) {
        for (int i = 0; i < num_medicines; i++) {
            pids[i]= getpid();
            medicines[i].id = i;
            medicines[i].type = getRandomValue(0,2, i); 
            printf("p[%d]the type is %d \n", medicines[i].id, medicines[i].type);
            if (medicines[i].type == LIQUID) {
                numberOfProductionLinesForLiquid++;  // Increment count for liquid
                if (!fork()) {

                    execlp("./productionLineLiquid", "productionLineLiquid", to_string(numberOfProductionLinesForLiquid).c_str(),to_string(pid).c_str(), nullptr);
                    exit(1);
                } 
            } else {
                numberOfProductionLinesForPills++; // Increment count for pills
                if (!fork()) {
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

    if ( sigset(SIGUSR1, signal_catcher) == SIG_ERR ) {
        perror("Sigset can not set SIGUSR1");
        exit(SIGUSR1);
    }

    while (1) {
        pause(); // Wait for signals
    }
    wait(NULL);
    wait(NULL);
}
