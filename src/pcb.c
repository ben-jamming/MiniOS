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
PCB* makePCB(char* fileName, int fileSize, int pageSize){
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pageSize = pageSize;
    // Nnumber of pages is the ceiling of the file size divided by the page size
    // This is because each page can hold 3 lines of code
    int pageTableSize = (int) ceil((double)fileSize / (double) pageSize);
    newPCB->pid = generatePID();
    newPCB->PC = 0;
    // newPCB->start  = start;
    // newPCB->end = end;
    newPCB->fileName = fileName;
    newPCB->fileSize = fileSize;
    newPCB->job_length_score = 1+fileSize;
    newPCB->pageTable = malloc(pageTableSize * sizeof(int));
    newPCB->priority = false;
    return newPCB;
}

void printPCB(PCB *pcb) {
    printf("PID: %d\n", pcb->pid);
    printf("PC: %d\n", pcb->PC);
    printf("File Name: %s\n", pcb->fileName);
    printf("File Size: %d\n", pcb->fileSize);
    printf("Job Length Score: %d\n", pcb->job_length_score);
    printf("Priority: %s\n", pcb->priority ? "true" : "false");
    printf("Page Table:\n");
    for(int i = 0; i < (int) sizeof(pcb->pageTable)/sizeof(pcb->pageTable[0]); i++) {
        printf("    %d\n", pcb->pageTable[i]);
    }
    printf("Done printing page table.");
    printf("\n");
}
