#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"
#include "dirent.h"

int MAX_ARGS_SIZE = 8;

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

int help();
int quit();
int set(char* var, char** value, int val);
int ls();
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();

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

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		return badcommand();
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
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "echo")==0){
		if (args_size < 2) return badcommand();
		for (int i = 1; i < args_size; i++){
			if (command_args[i][0] == '$'){
				//check for $
				char *var_name = get_all_but_first(command_args[i]);
				char *val = mem_get_value(var_name);
				printf("%s",val);
			}
			else{
				printf("%s",command_args[i]);
			};

		};
		printf("\n");
		return  0;
	} else if (strcmp(command_args[0], "ls")==0){
        if (args_size != 1) return badcommandTooManyTokens();

        ls();

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

void printStrings(char ** strings,int len) {
        int i = 0;
        while( i < len ) 
        {   
            printf("%s\n",strings[i]);
            i++;
        }
}

int ls(){
        char* directoryPath = ".";

        int itemCount = getDirSize(directoryPath);

        char** names = getDirNames(directoryPath, itemCount);

        qsort(names,itemCount,sizeof(char*),strSortComp);

        printStrings(names, itemCount);

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

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
