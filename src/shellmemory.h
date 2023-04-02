#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H
#include "pcb.h"
void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int load_file(FILE* fp, int* pStart, int* pEnd, char* fileID);
char * mem_get_value_at_line(int index);
void mem_free_lines_between(int start, int end);
void printShellMemory();
void evictFrame(PCB* pcb, int frameNum);
void assignFrame(PCB* pcb, int pageNum);
void loadPage(PCB* pcb, int pageNum);
int getLRUFrame();
int *getNextLine(PCB* pcb);
int getVictimFrame();
#endif