#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include"pcb.h"

#define SHELL_MEM_LENGTH 1000

void evictFrame(PCB* pcb, int frameNum);
void assignFrame(PCB* pcb, int pageNum);
void loadPage(PCB* pcb, int pageNum);
void freePageTableFrames(PCB* pcb);
int getLRUFrame();
char *getNextLine(PCB* pcb);
int getVictimFrame();
int getFreeFrame();
void initFrameStore();


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
PCB **frameStore;

// Initialize frame store
void initFrameStore(){
	frameStore = malloc(FRAME_STORE_SIZE * sizeof(PCB *));
	int i;
	for(i=0;i<FRAME_STORE_SIZE;i++){
		frameStore[i] = NULL;
	}
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


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
int load_file(FILE* fp, int* pStart, int* pEnd, char* filename)
{
	char *line;
    size_t i;
    int error_code = 0;
	bool hasSpaceLeft = false;
	bool flag = true;
	i=101;
	size_t candidate;
	while(flag){
		flag = false;
		for (i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") == 0){
				*pStart = (int)i;
				hasSpaceLeft = true;
				break;
			}
		}
		candidate = i;
		for(i; i < SHELL_MEM_LENGTH; i++){
			if(strcmp(shellmemory[i].var,"none") != 0){
				flag = true;
				break;
			}
		}
	}
	i = candidate;
	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (size_t j = i; j < SHELL_MEM_LENGTH; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
			line = calloc(1, SHELL_MEM_LENGTH);
            fgets(line, 999, fp);
			shellmemory[j].var = strdup(filename);
            shellmemory[j].value = strndup(line, strlen(line));
			free(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= SHELL_MEM_LENGTH; i ++){
			shellmemory[j].var = "none";
			shellmemory[j].value = "none";
    	}
		return error_code;
	}
	//printShellMemory();
    return error_code;
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
}

int getFreeFrame() {
  int freeFrame = -1;
  //TODO: CHANGE FRAME_STORE_SIZE to NUM_FRAMES (NUM_FRAMES=FRAME_STORE_SIZE/ frame size)
  for (int i=0; i<FRAME_STORE_SIZE; i++) {
    PCB* framePCB = frameStore[i];
    if (framePCB = NULL) {
      freeFrame = i;
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
}

// Load a page from the backing store into the frame store
//@param pcb: the PCB of the process
//@param pageNum: the page number to load
void loadPage(PCB* pcb, int pageNum){
	// Assign a frame to load the page into and add it to the page table
	assignFrame(pcb, pageNum);
	// Load the frame from the backing store
	// TODO: load the page from the backing store
	printf("Loading page %d into frame %d\n", pageNum, pcb->pageTable[pageNum]);
}

int getLRUFrame(){
	// get the least recently used frame
	return 0;
}
char *getNextLine(PCB* pcb){
	int frameNum = -1;
	int pageNum = pcb->PC/pcb->pageSize;
	// get the frame number of the current page
	// TODO: check if the page is in the frame store
	// For now, assume that the page is in the frame store
	frameNum = pcb->pageTable[pageNum];
	// get the next line of the program from the page store
	char *line = mem_get_value_at_line(frameNum + pcb->PC % pcb->pageSize);
	return line;
}
int getVictimFrame(){
	// get the victim frame to evict
	return 0;
}

bool pageFault(char *line, PCB* pcb){
	// check if there is a page fault at the current line
	if(line == NULL){
		// Make a call to get victim frame
		int victim_frame = getVictimFrame();
		// Evict the victim frame
		printf("Page fault! Victim page contents:\n");
		for(int i=0; i<pcb->pageSize; i++){
			printf("%s", mem_get_value_at_line(victim_frame*pcb->pageSize+i));
		}
		printf("End of victim page contents.\n");
		evictFrame(pcb,victim_frame);
		return true;
	}

	return false;
}
