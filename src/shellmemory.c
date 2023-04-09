#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include"pcb.h"
#include <stddef.h>
#include "lrucache.h"


#define SHELL_MEM_LENGTH 1000
#define PAGE_SIZE 3
#define FRAME_COUNT FRAME_STORE_SIZE/PAGE_SIZE

void evictFrame(PCB* pcb, int frameNum);
void assignFrame(PCB* pcb, int pageNum);
void loadPage(PCB* pcb, int pageNum);
void freePageTableFrames(PCB* pcb);
char *getNextLine(PCB* pcb);
int getVictimFrame();
int getRandomFrame();
int getFreeFrame();
void printEvictedFrame(int frameNum);
void initFrameStore();
int getLeastRecentFrame();


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
PCB **frameStore;

// Initialize frame store
void initFrameStore(){
	frameStore = malloc((FRAME_COUNT) * sizeof(PCB *));
	int i;
	for(i=0;i<FRAME_COUNT;i++){
		frameStore[i] = NULL;
	}

  createFrameTracker(FRAME_COUNT);
}

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){
	int i;
	for (i=0; i<1000; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=SHELL_MEM_LENGTH - VAR_STORE_SIZE; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=SHELL_MEM_LENGTH - VAR_STORE_SIZE; i<1000; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=0; i<1000; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}


void printShellMemory(){
	int count_empty = 0;
	for (int i = 0; i < SHELL_MEM_LENGTH; i++){
		if(strcmp(shellmemory[i].var,"none") == 0){
			count_empty++;
		}
		else{
			printf("\nline %d: key: %s\t\tvalue: %s\n", i, shellmemory[i].var, shellmemory[i].value);
		}
    }
	printf("\n\t%d lines in total, %d lines in use, %d lines free\n\n", SHELL_MEM_LENGTH, SHELL_MEM_LENGTH-count_empty, count_empty);
}

void freePageTableFrames(PCB* pcb) {
  //check to see that the pcb is not null
    if (pcb == NULL) {
    printf("Null pcb, cannot free frames\n");
    return;
  }

  //check to see that the tage table is not null
  if (pcb->pageTable == NULL) {
    printf("Null pageTable, cannot free frames\n");
    return;
  }

  //check the frame store is not null
  if (frameStore == NULL) {
    printf("Null frameStore, cannot free frames\n");
    return;
  }

  //if so loop over the page table
  int pageTableSize = pcb->pageTableSize;
  int* pageTable = pcb->pageTable;
  for (int i = 0; i < pageTableSize; i++) {
    int frameNum = pageTable[i];
    //for every non null entry, go to that frame in the frame store
    if (frameNum != -1) {
      frameStore[frameNum] = NULL;
    }
    //then set that frame store entry to null and set the page table entries to null
    pageTable[i] = -1;
  }

}


char * mem_get_value_at_line(int index){
	if(index<0 || index > SHELL_MEM_LENGTH) return NULL; 
	return shellmemory[index].value;
}

void mem_free_lines_between(int start, int end){
	for (int i=start; i<=end && i<SHELL_MEM_LENGTH; i++){
		if(shellmemory[i].var != NULL){
			free(shellmemory[i].var);
		}	
		if(shellmemory[i].value != NULL){
			free(shellmemory[i].value);
		}	
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Partition shell memory into frame store and variable store

void evictFrame(PCB* pcb, int frameNum){
	// evict the frame from the frame store

  //check the pcb is null
  if (pcb == NULL) {
    printf("Error: The pcb is NULL, cannot evict frame");
    return;
  }
  //check the pcb table is null
  if (pcb->pageTable == NULL) {
    printf("Error: PCB page table is null, cannot evict frame");
    return;
  }

  //get the pcbs page table
  int* pageTable = pcb->pageTable;
  //get the size of the page table
  int tableSize = pcb->pageTableSize;
  //loop over the page table entries
  for (int i=0;i<tableSize; i++) {
    //check the entry to see if it matches the frame number
    if (pageTable[i] == frameNum) {
      //if it does set that frame to -1 
      //evict frame
      pageTable[i] == -1;
      printEvictedFrame(frameNum);
      //break the loop
      return;
    }
  }
  //if frame number not found say so
  printf("Error: FrameNumber not found in page Table");
  return;
}

int getFreeFrame() {
  int freeFrame = -1;
  for (int i=0; i < FRAME_COUNT; i++) {
    PCB* framePCB = frameStore[i];
    if (framePCB == NULL) {
      freeFrame = i;
      break;
    }
  }
  return freeFrame;
}

void assignFrame(PCB* pcb, int pageNum){
	// assign a frame to the page
  if (pcb == NULL) {
    printf("Null pcb, cannot assign frame\n");
    return;
  }

  if (pcb->pageTable == NULL) {
    printf("Null pageTable, cannot assign frame\n");
    return;
  }

  if (pcb->pageTableSize <= pageNum) {
    printf("Page number too big for page table\n");
    return;
  }

  //get a free frame
  int freeFrame = getFreeFrame();

  //if a free frame is found then assign the frame the the page table in the pcb
  if (freeFrame != -1) {
    //assign the frame
    pcb->pageTable[pageNum] = freeFrame;
    frameStore[freeFrame] = pcb;
    return;
  }

  
  //if no free frame is found then get the victim frame
  int victimFrame = getVictimFrame();

  // get the pcb of the victimframe
  PCB* victimProcess = frameStore[victimFrame];


  //evict the frame (remove the frame number from page table)
  if (victimProcess != NULL) {
    evictFrame(victimProcess, victimFrame);
  }

  //assign the frame to the pcb
  pcb->pageTable[pageNum] = victimFrame;
  frameStore[victimFrame] = pcb;
}

void loadLinesFromFile(FILE* file,
        char* fileName,
        int frameNum,
        int pageNum) {
  
  int frameStart = frameNum * PAGE_SIZE;
  int pageStart = pageNum * PAGE_SIZE;
  int pageFinish = pageStart + PAGE_SIZE;

  //initialize buffer for holding current line and 
  int lineLength = 1000;
  char lineBuffer[lineLength];
  int currentLine = 0;
  while (fgets(lineBuffer, lineLength, file)
        && currentLine < pageFinish) {

    //check if we are at a line on the page
    if (currentLine >= pageStart) {
      //get the address that the line will be store in
      int pageLine = currentLine % PAGE_SIZE;//page line == frame line
      int lineAddress = frameStart + pageLine;
      shellmemory[lineAddress].var = strdup(fileName);
      shellmemory[lineAddress].value = strdup(lineBuffer);
    }
    currentLine++;
  } 
}

void replaceLineInShellMemory(PCB* pcb, char* line) {
  //get the page number and line number from the pcb
  int pageNum = pcb->PC / PAGE_SIZE;
  int lineNum = pcb->PC % PAGE_SIZE;
  // get the frame number from the page table
  int frameNum = pcb->pageTable[pageNum];
  int frameStart = frameNum * PAGE_SIZE;
  int pageStart = pageNum * PAGE_SIZE;
  int pageLine = lineNum % PAGE_SIZE;
  int lineAddress = frameStart + pageLine;
  //printf("Replacing line %s with %s", shellmemory[lineAddress].value, line);
  shellmemory[lineAddress].value = strdup(line);
}

// Load a page from the backing store into the frame store
//@param pcb: the PCB of the process
//@param pageNum: the page number to load
void loadPage(PCB* pcb, int pageNum){

    //set get program block variables
    int* pageTable = pcb->pageTable;
    int pageTableSize = pcb->pageTableSize;

    //checl that the page is inside the page table
    if (pageNum >= pageTableSize) {
      printf("Error: Page is out of bounds of page table\n");
      return;
    }

    //get the frame from the page
    int frameNum = pageTable[pageNum];

    //check to see that the page has a frame assigned
    if (frameNum == -1) {
      printf("Error: Page does not have an assigned Frame\n");
      return;
    }

    char* fileName = pcb->fileName;
    FILE *file = fopen(pcb->fileName, "r");

    if (file == NULL) {
      printf("Error: Could not open file:\n");
      return;
    }

    loadLinesFromFile(file, fileName, frameNum, pageNum);

    fclose(file);


}

char* getNextLine(PCB* pcb){


  int * pageTable = pcb->pageTable;
	int frameNum = -1;
  int PC = pcb->PC;
	int pageNum = PC / PAGE_SIZE;
  int pageLine = PC % PAGE_SIZE;

	// get the frame number of the current page
	frameNum = pageTable[pageNum];


  //check that the page has an assigned frame
  if (frameNum == -1) {
    return NULL;
  }

  int lineAddress = frameNum * PAGE_SIZE + pageLine;

	// get the next line of the program from the page store
  if (frameNum >= FRAME_COUNT) {
    printf("Frame out of bounds");
  }
  moveToFront(frameNum);
	char *line = mem_get_value_at_line(lineAddress);
	return line;
}

int getRandomFrame() {
  //get a random frame out of the set of frames
  int min = 0;
  //TODO: this will haved to be changed to framestore/page size
  int max = FRAME_COUNT;
  
  int randFrame = rand() % (max - min + 1) + min;
  // printf("randFrame:%d\n", randFrame);
  return randFrame;
}

int getLeastRecentFrame() {
  return getLRUFrame();
}

int getVictimFrame(){
	// get the victim frame to evict
  //TODO: add LRU policy
  //for now we will return a random number 
	// return getRandomFrame();
  return getLeastRecentFrame();
}

void printEvictedFrame(int victim_frame){
	// check if there is a page fault at the current line


		// Evict the victim frame
		printf("Page fault! Victim page contents:\n");
		for(int i=0; i<PAGE_SIZE; i++){
			printf("%s", mem_get_value_at_line(victim_frame*PAGE_SIZE+i));
		}
		printf("End of victim page contents.\n");

	return ;
}

void printFrame(int frameNum){
  int frameStart = frameNum * PAGE_SIZE;
  int frameFinish = frameStart + PAGE_SIZE;
  printf("Frame: %d\n", frameNum);
  printf("%s",shellmemory[frameStart].var);
  printf("\n");
  for (int i = frameStart; i < frameFinish; i++) {
    printf("%s",shellmemory[i].value);
  }
  printf("\n");
}

