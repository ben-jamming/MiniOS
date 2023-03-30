#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"
#include <math.h>

int pid_counter = 1;

int generatePID(){
    return pid_counter++;
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(char* fileName, int fileSize){
    PCB * newPCB = malloc(sizeof(PCB));
    int pageSize = 3;
    int pageTableSize = (int) ceil((double )num / (double) pageSize);
    newPCB->pid = generatePID();
    newPCB->PC = 0;
    // newPCB->start  = start;
    // newPCB->end = end;
    newPCB->fileName = fileName;
    newPCB->fileSize = fileSize;
    newPCB->job_length_score = 1+fileSize;
    newPCB->pageTable = (int *) malloc(pageTableSize * sizeof(int))
    newPCB->priority = false;
    return newPCB;
}