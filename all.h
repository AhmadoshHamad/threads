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



int readFromFile(std::string );

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


using namespace std;

bool getRandom(int min_range, int max_range,int seed) {
  
    srand((unsigned) getpid()+seed);
  
    int value = min_range + (rand() % (max_range - min_range));
    if(value > (min_range+max_range)/2){
        return true;
    }
    return false;
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