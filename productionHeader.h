#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/types.h>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
using namespace std;
int readFromFile(std::string parameter);
int numberOfMedicinesProduced = readFromFile("numberOfMedicinesProduced=");
int numberOfProductionLinesForLiquid = readFromFile("numberOfProductionLinesForLiquid=");
int numberOfProductionLinesForPills = readFromFile("numberOfProductionLinesForPills=");
int minLiquidLevelRange=readFromFile("minLiquidLevelRange=");
int maxLiquidLevelRange=readFromFile("maxLiquidLevelRange=");
int minLiquidColor=readFromFile("minLiquidColor=");
int maxLiquidColor=readFromFile("maxLiquidColor=");
int minChanceBottleSealed=readFromFile("minChanceBottleSealed=");
int maxChanceBottleSealed=readFromFile("maxChanceBottleSealed=");
int minChanceLabel=readFromFile("minChanceLabel=");
int maxChanceLabel=readFromFile("maxChanceLabel=");
int numberOfPillsPerPlastic=readFromFile("numberOfPillsPerPlastic=");
int minChanceColorSize=readFromFile("minChanceColorSize=");
int maxChanceColorSize=readFromFile("maxChanceColorSize=");
int minChanceExpiryDate=readFromFile("minChanceExpiryDate=");
int maxChanceExpiryDate=readFromFile("maxChanceExpiryDate=");
int numberOfEmployees = readFromFile("numberOfEmployees=");
int readFromFile(std::string parameter) {
    std::string value = "";
    std::ifstream inputFile("arguments.txt"); // Replace "vars.txt" with your file name
    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
        return -1; // Return a default value indicating error
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        size_t equalPos = line.find(parameter);

        if (equalPos != std::string::npos) { // If parameter found
            value = line.substr(equalPos + parameter.length());
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
    return -1; // Return
}
key_t shm_keyLiquids = ftok("/tmp", 'M'); 
key_t shm_keyPills = ftok("/tmp", 'N');
bool getRandom(int min_range, int max_range) {
  
    srand((unsigned) getpid());
  
    int value = min_range + (rand() % (max_range - min_range));
    if(value > (min_range+max_range)/2){
        return true;
    }
    return false;
}
struct bottle{
    bool level;
    bool color;
    bool sealed;
    bool label;
    bool expiryDate;
    int pid;
};
struct bottles{
    struct bottle bottle[100];
};
struct plastic_container{
    bool notMissingPills;
    bool colorAndSIze;
    bool expiryDate;
    int pid;
};
struct pills{
    struct plastic_container plastic_container[100];
};

int shmid_Bottles = shmget(shm_keyLiquids, sizeof(struct bottles), IPC_CREAT | 0666);

struct bottles *bottles_ptr = (struct bottles *)shmat(shmid_Bottles, NULL, 0);

int shmid_Pills = shmget(shm_keyPills , sizeof(struct pills), IPC_CREAT | 0666);

struct pills *pills_ptr = (struct pills *)shmat(shmid_Pills, NULL, 0);
sem_t *sem = sem_open("/mysemaphore", O_CREAT, 0644, 1);