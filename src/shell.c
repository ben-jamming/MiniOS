#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
//#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"

int interpreterMode(int fd){
    //fd = 'file descriptor'
    //return 1 if reading from terminal (interpreter mode)
    //return 0 otherwise (batch mode)
    return isatty(fd);
}

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);
int parseCommands(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
	printf("%s\n", "Shell version 1.2 Created January 2023");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();
	while(1) {			
        
        int interpreter_mode = interpreterMode(fileno(stdin));
        			
		if (interpreter_mode){
            //Only display '$' if in interpreter mode
            printf("%c ",prompt);
        }
        
		fgets(userInput, MAX_USER_INPUT-1, stdin);
		errorCode = parseCommands(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors

        //End of file reached and in batch mode
        //Open terminal file and enter interpreter mode
        if(feof(stdin) && !interpreter_mode){
            FILE *fp = freopen("/dev/tty", "r", stdin);
        }
        
		memset(userInput, 0, sizeof(userInput));
	}

	return 0;

}

int parseCommands(char ui[]) {

    //loop through all characters in the user input

    //if the character is ';'
    int result;
    int i = 0; //loops through every character
    int j = i; //keeps track of startering char for command
    int k = 0; //command count
    while ( k < 10 && i < (MAX_USER_INPUT) ) {
        
        // if we reach end of line or command , execute command
        if (ui[i] == ';' || ui[i]=='\0' || ui[i]=='\n') {
            if ( ui[i]=='\n' || j != i ) {
                result = parseInput(&ui[j]);
                if (result != 0 ) {
                    return result;
                }
            }
            //move starter to next char
            j = i+1;
            k++;
        }
        i ++;
    }
    return result;
}

int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID    
    int errorCode;
    for(a=0; ui[a]==' ' && a<MAX_USER_INPUT; a++);        // skip white spaces
    while(ui[a] != '\n' && ui[a] != '\0' && a<MAX_USER_INPUT) {
        
        for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<MAX_USER_INPUT; a++, b++){
            tmp[b] = ui[a];                        
            // extract a word
        }
        tmp[b] = '\0';
        words[w] = strdup(tmp);
        w++;
        if(ui[a] == '\0' || ui[a] == ';') break;
        a++; 
    }
    errorCode = interpreter(words, w);
    return errorCode;
}




