#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H
int initBackingStore();
int removeBackingStore();
void writeToBackstore(PCB* pcb, char *filename);
#endif