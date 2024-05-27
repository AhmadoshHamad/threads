#include "productionHeader.h"
int main(){
    printf("raziii");
    int array[numberOfProductionLinesForLiquid+numberOfProductionLinesForPills];
    int i=0;
    for (int i = 0; i < numberOfProductionLinesForLiquid; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            cerr << "Fork failed for production line " << i << std::endl;
            return 1;
        } else if (pid == 0) {
            string index_str = to_string(i);

            execlp("./liquidProduction", "liquidProduction", to_string(i).c_str(), nullptr);
            exit(1);
        } else {
            array[i] = pid;
        }
    }
    for (i= numberOfProductionLinesForLiquid; i < numberOfProductionLinesForPills+numberOfProductionLinesForLiquid; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            cerr << "Fork failed for production line " << i << std::endl;
            return 1;
        } else if (pid == 0) {
            string index_str = to_string(i);
            execlp("./pillProduction", "pillProduction", to_string(i).c_str(), nullptr);
            exit(1);
        } else {
            array[i] = pid;
        }
    }

    for (int i = 0; i < numberOfProductionLinesForPills+numberOfProductionLinesForLiquid; ++i) {
        int status;
        pid_t pid = array[i];
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "Error waiting for child process " << pid << std::endl;
        } else if (WIFEXITED(status)) {
            std::cout << "Child process " << pid << " exited with status " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cout << "Child process " << pid << " did not exit normally" << std::endl;
        }
    }
    if(shmdt(bottles_ptr)==-1){
        perror("Bottles");
        exit(1);
    }
    if(shmdt(pills_ptr)==-1){
        perror("Pills");
        exit(1);
    }
    if(shmctl(shm_keyLiquids, IPC_RMID, NULL)==-1){
        perror("Bottles");
        exit(1);
    }
    if(shmctl(shm_keyPills, IPC_RMID, NULL)==-1){
        perror("Pills");
        exit(1);
    } 
    if (sem_close(sem) == -1) {
    std::cerr << "Error closing semaphore" << std::endl;
    exit(1);
    }
    if (sem_unlink("/mysemaphore") == -1) {
        std::cerr << "Error unlinking semaphore" << std::endl;
        exit(1);
    }
    return 0;
}