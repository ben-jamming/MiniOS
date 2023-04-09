#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H
#define SHELL_MEM_LENGTH 1000
#define PAGE_SIZE 3 
#define FRAME_COUNT FRAME_STORE_SIZE/PAGE_SIZE
#include "pcb.h"
void mem_init();
void initFrameStore();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
char * mem_get_value_at_line(int index);
void mem_free_lines_between(int start, int end);
void printShellMemory();
void evictFrame(PCB* pcb, int frameNum);
void assignFrame(PCB* pcb, int pageNum);
void freePageTableFrames(PCB* pcb);
void loadPage(PCB* pcb, int pageNum);
int getLRUFrame();
char *getNextLine(PCB* pcb);
int getVictimFrame();
int getRandomFrame();
int getFreeFrame();
void printEvictedFrame(int frameNum);
void printFrame(int frameNum);
void replaceLineInShellMemory(PCB *pcb, char* line);
#endif