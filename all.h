#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctime>
#include <fstream>
#include <random>
#include <semaphore.h>
#include <signal.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#define fifo_lead "fifo"

struct Parameters{
    int success=0;
    int bottleFail=0;
    int pillFail=0;
};
int readFromFile(std::string );
int pids[10];
int numberOfProductionLines = readFromFile("numberOfProductionLines=");
int numberOfMedicinesProduced = readFromFile("numberOfMedicinesProduced=");
int minLiquidLevelRange=readFromFile("minLiquidLevelRange=");
int maxLiquidLevelRange=readFromFile("maxLiquidLevelRange=");
int minLiquidColor=readFromFile("minLiquidColor=");
int maxLiquidColor=readFromFile("maxLiquidColor=");
int minChanceBottleSealed=readFromFile("minChanceBottleSealed=");
int maxChanceBottleSealed=readFromFile("maxChanceBottleSealed=");
int minChanceLabel=readFromFile("minChanceLabel=");
int maxChanceLabel=readFromFile("maxChanceLabel=");
int minChanceColorSize=readFromFile("minChanceColorSize=");
int maxChanceColorSize=readFromFile("maxChanceColorSize=");
int minChanceExpiryDate=readFromFile("minChanceExpiryDate=");
int maxChanceExpiryDate=readFromFile("maxChanceExpiryDate=");
int numberOfEmployees = readFromFile("numberOfEmployees=");
int minChanceNumberOfPills = readFromFile("minChanceNumberOfPills=");
int maxChanceNumberOfPills = readFromFile("maxChanceNumberOfPills=");
int numEmp = readFromFile("numOfEmp=");
int runningTime = readFromFile("runningTime=");
int numberOfFailedBottledMedicine = readFromFile("numberOfFailedBottledMedicine=");
int numberOfFailedPillMedicine = readFromFile("numberOfFailedPillMedicine=");
int minSpeed = readFromFile("minSpeed=");
int maxSpeed= readFromFile("maxSpeed=");
int threshold = readFromFile("threshold=");
key_t shm_keyParameters = ftok("/tmp", 'R'); // Using a constant value as the second argument for uniqueness

int shmid_Parameters = shmget(shm_keyParameters, sizeof(struct Parameters), IPC_CREAT | 0666);
// Attach to the shared memory segment
struct Parameters *parameter_ptr = (struct Parameters *)shmat(shmid_Parameters, NULL, 0);

typedef struct {
    int openID;
    int signal;
    int productionLine;
} ThreadArgs;

using namespace std;

void sendSignal(int pid, int lineNumber, int signal) {
    union sigval value;
    value.sival_int = lineNumber;  // Set the line number

    // Send the signal to the specified process
    if (sigqueue(pid, signal, value) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }
}
void cleanup_shared_memory() {
    parameter_ptr->success =0;
    parameter_ptr->pillFail =0;
    parameter_ptr->bottleFail=0;
    // Mark the shared memory segment for deletion
    if (shmctl(shmid_Parameters, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    } else {
        printf("Shared memory marked for deletion.\n");
    }
}
void end(int pid){
    printf("Success: %d\n Failed Bottles:%d\n Failed Pills: %d\n", parameter_ptr->success, parameter_ptr->bottleFail, parameter_ptr->pillFail);
  cleanup_shared_memory();
    printf("Timer expired\n");
    kill(pid, SIGTERM);
    for (int i = 0; i < 11; i++) {
        if (kill(pids[i], SIGTERM) == -1) {
            perror("error terminate");
        } else {
            printf("Sent SIGTERM to process %d\n", pids[i]);
        }
    }

    // Wait for all child processes to terminate
    for (int i = 0; i < 11; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) == -1) { 
            perror("waitpid");
        } else {
            printf("Process %d terminated with status %d\n", pids[i], status);
        }
    }
    exit(0);
}
// Thread function to send a signal
void* threadFunction(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int openID = args->openID;
    int signal = args->signal;
    int line = args->productionLine;
    sendSignal(openID, line, signal);
    free(args); // Free the allocated memory for arguments
    return NULL;
}
void sendingSignal(int openID, int signal, int productionLine){
    pthread_t thread;
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));  // Cast malloc's return value to ThreadArgs*
    if (args == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Set the arguments
    args->openID = openID;
    args->signal = signal;
    args->productionLine = productionLine;

    // Create the thread and pass the arguments
    if (pthread_create(&thread, NULL, threadFunction, (void*)args) != 0) {
        perror("pthread_create");
        free(args); // Free the allocated memory in case of an error
        exit(EXIT_FAILURE);
    }
}
bool getRandom(int min_range, int max_range,int seed) {
  
    srand((unsigned) getpid()+seed);
  
    int value = min_range + (rand() % (max_range - min_range));
    if(value > (min_range+max_range)/2){
        return true;
    }
    return false;
}
int getRandomValue(int min_range, int max_range,int seed) {
  
    srand((unsigned) getpid()+seed);
  
    int value = min_range + (rand() % (max_range - min_range));
    return value;
}

int readFromFile(std::string parameter) {
    std::string value = "";
    std::ifstream inputFile("vars.txt"); // Replace "vars.txt" with your file name
    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
        return -1; // Return a default value indicating error
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        size_t equalPos = line.find(parameter);

        if (equalPos != std::string::npos) { // If parameter found
            // Extract the substring after the parameter
            value = line.substr(equalPos + parameter.length());

            // Print for debugging
            // std::cout << "Parameter: " << parameter << ", Extracted value: " << value << std::endl;

            // Convert the value string to an integer
            inputFile.close(); // Close the file before returning
            try {
                return std::stoi(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
                return -1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << std::endl;
                return -1;
            }
        }
    }
    inputFile.close(); // Close the file

    std::cerr << "Parameter not found in the input file." << std::endl;
    return -1; // Return a default value indicating error
}
// struct sembuf lock = {0, -1, SEM_UNDO}; // Semaphore lock operation
// struct sembuf unlock = {0, 1, SEM_UNDO}; // Semaphore unlock operation

void reset(){
    system(" rm  /dev/shm/*");
}

#define MEMORY_SIZE  4096