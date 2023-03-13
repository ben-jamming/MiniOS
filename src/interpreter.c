#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "shellmemory.h"
#include "shell.h"
#include "dirent.h"

int MAX_ARGS_SIZE = 7;

// Define the process states
enum process_state {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// Define set of policies
const char* policies[] = {"FCFS","SJF","RR","AGING"};

// Define the process control block
struct PCB {
    int pid;                  // process ID
    int program_location;     // address of first line
    int program_counter;      // address of current line
    int num_lines;            // number of lines in program
    enum process_state state; // current process state
    int priority;             // process scheduling priority
    struct PCB *next;         // pointer to next PCB in ready queue
};

// Define the scheduling policy
typedef enum {
    FCFS, 
    SJF,
    RR,
    AGING
} Policy;
// Define a mapping from strings to enum values
const struct {
    const char *string;
    Policy policy;
} policy_map[] = {
    {"FCFS", FCFS},
    {"SJF", SJF},
    {"RR", RR},
    {"AGING", AGING}
};
// Define the ready queue
struct PCB *ready_queue_head = NULL;
struct PCB *ready_queue_tail = NULL;

// Helper function to display contents of PCB
void print_pcb_contents(struct PCB *pcb){
    printf("PID of this PCB: %d\n",pcb->pid);
    printf("Address of the first line of program stored in this PCB: %d\n",pcb->program_location);
    printf("Address of current line (program counter) of this PCB: %d\n",pcb->program_counter);
    printf("Total number of lines in this PCB: %d\n", pcb->num_lines);
    printf("Process state of this PCB: %d\n",pcb->state);
    printf("Priority of this PCB: %d\n",pcb->priority);
    if(pcb->next == NULL){
        // Head of the queue
        printf("This PCB is at the tail of the queue\n");
    }
    else{
        printf("PID of next PCB: %d\n",pcb->next->pid);
    }

}

// Helper function to print ready_queue
void print_ready_queue(struct PCB *current_pcb) {
    printf("Ready Queue: ");
    while (current_pcb != NULL) {
        printf("%d -> ", current_pcb->pid);
        current_pcb = current_pcb->next;
    }
    printf("NULL\n");
}


// Add a new PCB to the tail of the ready queue
void add_to_ready_queue(struct PCB *pcb) {
    pcb->next = NULL;
    if (ready_queue_head == NULL) {
        // If the queue is empty, set both head and tail to the new PCB
        ready_queue_head = pcb;
        ready_queue_tail = pcb;
    } else {
        // Otherwise, add the new PCB to the tail of the queue
        ready_queue_tail->next = pcb;
        ready_queue_tail = pcb;
    }
}

// Remove the head PCB from the ready queue and return it
struct PCB *remove_from_ready_queue() {
    if (ready_queue_head == NULL) {
        // If the queue is empty, return NULL
        return NULL;
    } else {
        // Otherwise, remove the head PCB and return it
        struct PCB *removed_pcb = ready_queue_head;
        ready_queue_head = ready_queue_head->next;
        //if(removed_pcb != NULL){printf("PCB being removed is %d\n", removed_pcb->pid);}
        //printf("Next head of the queue is %d\n",ready_queue_head->pid);
        //print_ready_queue(ready_queue_head);
        if (ready_queue_head == NULL) {
            // If the queue is now empty, set the tail to NULL as well
            ready_queue_tail = NULL;
        }
        return removed_pcb;
    }
}

// Set up the PCB for a new process
void setup_process(struct PCB *pcb, int pid, int program_location, int num_lines) {
    // Set the PCB properties
    // TODO:
    // Configure the state and priority members to be dynamic
    pcb->pid = pid;
    pcb->program_location = program_location;
    pcb->program_counter = program_location;
    pcb->num_lines = num_lines;
    pcb->state = READY;
    pcb->priority = 0;
    pcb->next = NULL;
}

// Run the program stored in a given PCB
int run_program(struct PCB *pcb) {
    char next_addr[4];
    sprintf(next_addr, "%d", pcb->program_counter);  // Initialize next_addr to program_counter
    // Error code
    int errorCode = 0;
    // Set the program counter to the address of the first line
    int program_counter = pcb->program_location;
    char line[1000];
    // Loop over each line in the program
    while (program_counter < pcb->num_lines+pcb->program_location) {
        // Get the next line from memory
        memset(line, '\0', sizeof(line));
        sprintf(next_addr,"%d", program_counter);
        strcpy(line, mem_get_value(next_addr));
        //printf("The value retrieved from %s is: %s\n",next_addr,line);
        // Interpret the line using parseInput
        errorCode = parseInput(line);

        // Increment the program counter to the next line
        program_counter ++;

        // Clear space for next line address
        memset(next_addr, '\0', sizeof(next_addr));
    }
    // Free shell memory occupied by PCB
    
    mem_reset(pcb->program_location, pcb->num_lines);
    
    return errorCode;
}


// Scheduler function
void scheduler(Policy policy) {

    // Run processes in the ready queue until there are no more processes
    while (ready_queue_head != NULL) {
        // Get the next process from the ready queue based on the scheduling policy
        struct PCB *next_pcb;
        switch(policy){
            case FCFS:
                // FCFS policy: remove the head of the queue
                next_pcb = remove_from_ready_queue();
                break;
            case SJF:
                // SJF policy: remove the process with the shortest estimated job length
                // Initialize variables to keep track of the PCB with the shortest job length and its estimated length
                int shortest_job_length = ready_queue_head->num_lines;
                struct PCB *shortest_job_pcb = ready_queue_head;
                // Loop over the remaining PCBs in the ready queue
                struct PCB *current_pcb = ready_queue_head->next;
                while(current_pcb != NULL){
                    // Calculate the estimated job length for the current PCB
                    int current_job_length = current_pcb->num_lines;
                    // If the current PCB has a shorter job length than the current shortest job length, update the variables
                    if(current_job_length < shortest_job_length){
                        shortest_job_length = current_job_length;
                        shortest_job_pcb = current_pcb;
                    }
                    // Move on to the next PCB in the queue
                    current_pcb = current_pcb->next;
                }
                // Remove the PCB with the shortest job length from the ready queue
                next_pcb = shortest_job_pcb;
                remove_from_ready_queue(next_pcb);
                break;
            case RR:
                // RR policy: remove the head of the queue and add it to the tail after a fixed number of instructions
                next_pcb = remove_from_ready_queue();
                // Run the process for a fixed number of instructions (in this case, 2) or until it terminates
                int num_instructions = 0;
                while(num_instructions < 2 && next_pcb->state != TERMINATED){
                    run_program(next_pcb);
                    num_instructions ++;
                }
                // If the process has not terminated, add it back to the tail of the ready queue
                if(next_pcb->state != TERMINATED){
                    add_to_ready_queue(next_pcb);
                }
                break;
            case AGING:
                // TODO: implement aging policy
                next_pcb = remove_from_ready_queue();
                break;
            default:
                printf("Invalid scheduling policy\n");
                return;
        }
        // Run the process
        run_program(next_pcb);
        // Clean up the process
        free(next_pcb);
    }
}






int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}


int badcommandTooManyTokens(){
    printf("%s\n", "Bad command: Too many tokens");
	return 4;
}

int badcommandUnableToReadDirectory(){
    printf("%s\n", "Bad command: Unable to read directory");
    return 5;
}

int badcommandMkDir(){
    printf("%s\n", "Bad command: my_mkdir");
    return 6;
}

int badcommandCD() {
    printf("%s\n", "Bad command: my_cd");
    return 8;
}
int badcommandCannotWriteFile(){
	printf("%s\n", "Bad command: Cannot write file");
	return 7;
}

int badcommandExecPolicies(){
    printf("%s\n"," Invalid policy: exec");
    return 9;
}

int badCommandDuplicateScripts(){
    printf("%s\n","Bad command: same file name");
    return 10;
}

int badCommandNotEnoughMemory(){
    printf("%s\n","Error: Insufficient space in shell memory");
    return 11;
}

int help();
int quit();
int set(char* var, char** value, int val);
int my_cd(char* dirName);
int ls();
int my_mkdir(char* dirName);
int touch(char* filepath);
char* subVar(char* token);
int print(char* var);
int run(char** command_args, int arg_size);
int badcommandFileDoesNotExist();
int exec(char** command_args, int arg_size);

char* get_all_but_first(char* str) {
    int str_len = strlen(str);
    char* all_but_first = (char*)malloc(str_len);
    strncpy(all_but_first, str + 1, str_len - 1);
    all_but_first[str_len - 1] = '\0';
    return all_but_first;
}
// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 ){
		return badcommand();
	}
    if( args_size > MAX_ARGS_SIZE ) {
        return badcommandTooManyTokens();
    }

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3 || args_size > 7 ) return badcommandTooManyTokens();	
		return set(command_args[1], command_args, args_size);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {

		if (args_size > 2) return badcommand();

		return run(command_args,args_size);
	
	} else if (strcmp(command_args[0], "echo")==0){
		if (args_size < 2) return badcommand();
		for (int i = 1; i < args_size; i++){
            char* token = command_args[i];
            printf("%s", subVar(token));
		};
		printf("\n");
		return  0;
	} else if (strcmp(command_args[0], "my_cd")==0) {
        if (args_size != 2) return badcommandTooManyTokens();

        my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "my_ls")==0){
        if (args_size != 1) return badcommandTooManyTokens();

        ls();

    } else if (strcmp(command_args[0], "my_mkdir")==0) {
        if (args_size != 2) return badcommandTooManyTokens();

        my_mkdir(command_args[1]);

    } else if (strcmp(command_args[0], "my_touch")==0) {
        if (args_size != 2) return badcommandTooManyTokens();

        touch(command_args[1]);

    } 
    else if (strcmp(command_args[0], "exec")==0){
        if (args_size > 5 || args_size < 3) return badcommandTooManyTokens();
        
        exec(command_args,args_size);
    }
    else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int strSortComp (const void * elem1, const void * elem2) 
{
    char** f1 = ((char**)elem1);
    char** f2 = ((char**)elem2);
    return strcmp(*f1,*f2);
} 

int getDirSize(char* directoryPath) {
    //Open directory and read contents
    DIR *directory;

    directory = opendir(directoryPath);
    //handle errors
    if(directory == NULL){
        return badcommandUnableToReadDirectory();
    }

    int itemCount = 0 ;

    struct dirent *item =readdir(directory);//directory or file

    //count the number of files , used for prepping mem allocation
    while( item != NULL ) 
    {   
        itemCount++;
        item =readdir(directory);
    }
    closedir(directory);

    return itemCount;
}

char** getDirNames(char* directoryPath,int len) {
        int itemCount = len;
        
        //make space for pointers to names
        char** names = malloc(itemCount *  sizeof(char *));

        DIR *directory;
        directory = opendir(".");

        //handle errors
        if(directory == NULL){
            badcommandUnableToReadDirectory();
        }

        struct dirent *item =readdir(directory);
        //can be directory or file

        //load file names into array
        int i = 0;
        while( item != NULL ) 
        {   
            names[i] = item->d_name; 
            item =readdir(directory);
            i++;
        }
        closedir(directory);

        return names;

}

int shownNamesLength(char** names,int len) {

    //count non hidden names
    int i = 0;
    int shownNamesLen = 0;
    while( i < len ) 
    {   
        char* direntName = names[i];
        if ( direntName[0] != '.') {
            shownNamesLen++;
        }
        i++;
    }

    return shownNamesLen;

}

char** removeHiddenNames(char** names,int len) {
        int shownNamesLen = shownNamesLength(names, len);

        char** shownNames = malloc(shownNamesLen *  sizeof(char *));

        //count non hidden names
        int i = 0;
        int j = 0;
        while( i < len ) 
        {   
            char* direntName = names[i];
            if ( direntName[0] != '.') {
                shownNames[j] = names[i];
                j++;
            }
            i++;
        }

        return shownNames;
}

void printStrings(char** strings,int len) {
        int i = 0;
        while( i < len ) 
        {   
            printf("%s\n",strings[i]);
            i++;
        }
}

char* subVar(char* token) {
        if (token[0] == '$'){
            //check for $
            char *var_name = get_all_but_first(token);
            char *val = mem_get_value(var_name);
            return val;
        }
        else{
            return token;
        };
}

int ls(){
        char* directoryPath = ".";

        int itemCount = getDirSize(directoryPath);

        char** names = getDirNames(directoryPath, itemCount);

        qsort(names,itemCount,sizeof(char*),strSortComp);

        int shownNamesLen = shownNamesLength(names,itemCount);

        char** shownNames = removeHiddenNames(names, itemCount);

        printStrings(shownNames, shownNamesLen);
        // printStrings(names, itemCount);
        

        free(names);
        free(shownNames);

        return 0;
}

int touch(char* filepath){
    FILE* newFile = fopen(filepath,"w");

    if (newFile == NULL) {
        return badcommandCannotWriteFile();
    }

    fclose(newFile);
    return 0;
}

int my_mkdir(char* dirName) {

    //replace any variables with their value
    char* fullDirName = subVar(dirName);

    //check if the variable didnt exist, make directory and check for errors
    //if errors return bad command
    if (fullDirName == NULL || mkdir(fullDirName, 0777) == -1 ) {
        return badcommandMkDir();
    }
    return 0;
}

int my_cd(char* dirName) {
    
    if (chdir(dirName)) {
        return badcommandCD();
    }
    return 0;
}

int set(char* var, char** value, int argsize){
    int bufferSize = 1000;
	char *link = " ";
	char buffer[bufferSize];

    //clear the buffer
    memset(buffer, '\0', bufferSize * sizeof(buffer[0]));

    //loop through the argument pointers, starting at the first 
    for (int i=2;i<argsize;i++) {
        strcat(buffer, value[i]); //concatonate additional arguments
        //if this is not the last argument, add a space
        if (i<(argsize-1)) {
            strcat(buffer, link);
        }
    }
    //store in memory
	mem_set_value(var, buffer);
	return 0;
}


int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char** command_args, int arg_size){

    // TODO:
    // Check validity of arguments passed into run
	int errCode = 0;
    // Copied implementation from set
    char line[1000];
    // Initialize start address var
    int pcb_start_addr;
    memset(line, '\0', 1000 * sizeof(line[0]));
    //Loop over each program passed as argument
    for (int arg = 1; arg < arg_size; arg++){

        FILE *p = fopen(command_args[arg],"rt");  // the program is in a file
        //printf("argument %d: %s\n",arg,command_args[arg]);
        
        if(p == NULL){
            
            return badcommandFileDoesNotExist();
            
        }

        // Create a new PCB
        struct PCB *new_pcb = malloc(sizeof(struct PCB));
        // Find space in memory to store program contents
        pcb_start_addr = mem_find_first_free();
        // Throw error if space there's insufficient space
        if (pcb_start_addr == -1){return badCommandNotEnoughMemory();}
        // Loop over each line of the program and save each into a different memory location
        int line_num = 0;
        // Address as a string (should be < 3 digits)
        char str_addr[4];
        while (fgets(line, 999, p)) {
            // Calculate the address in memory for this line
            int address = pcb_start_addr+line_num;
            //printf("Address of memory slot for line number %d is: %d\n",line_num,address);
            // Store the line in memory at this address
            sprintf(str_addr,"%d",address);
            //printf("line to be stored at %d is: %s\n",address, line);
            mem_set_value(str_addr, line);
            //printf("For comparison, the value retrieved from %s is: %s\n",str_addr,mem_get_value(str_addr));
            // Move to the next line and reset address array and line buffer
            line_num++;
            memset(str_addr, '\0', sizeof(str_addr));
            memset(line, '\0', sizeof(line));
            
        }

        //printf("----------Done loading the lines of this file---------\n");
        // Configure the new PCB
        setup_process(new_pcb, pcb_start_addr, pcb_start_addr, line_num);
        //print_pcb_contents(new_pcb);
        // Add the initial process to the ready queue
        add_to_ready_queue(new_pcb);
        
        fclose(p);
        }
    //print_ready_queue(ready_queue_head);
    // Run Scheduler (currently with FCFS policy)
    //printf("----------------FINISHED LOADING PROGRAMS: RUNNING SCHEDULER----------------\n");
    

	return errCode;
}
int validate_policy(char policy_choice[]){
    int errCode = 0;
    int length = sizeof(policies)/sizeof(policies[0]);
    // Check if policy is valid
    for (int p = 0; p < length; p++){
        // Give the green light if policy is valid
        if (strcmp(policy_choice, policies[p]) == 0){
            return errCode;
        }
    }
    // Exit and return error if policy is invalid
    errCode = badcommandExecPolicies();
    return errCode;
}
// Look up an enum value from a string
Policy get_policy(const char *string) {
    for (int i = 0; i < sizeof(policy_map) / sizeof(policy_map[0]); i++) {
        if (strcmp(string, policy_map[i].string) == 0) {
            return policy_map[i].policy;
        }
    }
    // Return a default value or raise an error if the string is not found
    return FCFS;
}
int exec(char** command_args, int arg_size){
    // Check if policy is valid
    int errCode = validate_policy(command_args[arg_size-1]);
    if (errCode != 0){ return errCode;}
    // If valid, save policy
    char* policy_str = command_args[arg_size-1];
    //memcpy(policy_str,command_args[arg_size-1], sizeof(command_args[arg_size-1]));
    // create a new array with all but first and last elements
    char** new_command_args = malloc((arg_size - 1) * sizeof(char*));
    for (int i = 0; i < arg_size - 1; i++) {
        new_command_args[i] = command_args[i];
    }
    // Check if any of the elements in new_command_args are identical
    for (int i = 0; i < arg_size - 2; i++) {
        for (int j = i+1; j < arg_size - 1; j++) {
            if (strcmp(new_command_args[i], new_command_args[j]) == 0) {
                // Found identical processes
                free(new_command_args);
                return badCommandDuplicateScripts();
            }
        }
    }
    run(new_command_args,arg_size-1);
    Policy policy = get_policy(policy_str);
    scheduler(policy);
    free(new_command_args);
    return errCode;
}
