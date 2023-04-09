#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H
int initBackingStore();
int removeBackingStore();
void writeToBackstore(int pid, char *filename);
#endif