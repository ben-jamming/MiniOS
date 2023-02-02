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
		errorCode = parseInput(userInput);
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

int parseInput(char ui[]) {
    char tmp[200];
    char *words[100];                            
    int a = 0;
    int b;                            
    int w=0; // wordID    
    int errorCode;
    for(a=0; ui[a]==' ' && a<1000; a++);        // skip white spaces
    while(ui[a] != '\n' && ui[a] != '\0' && a<1000) {
        for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!='\n' && ui[a]!=' ' && a<1000; a++, b++){
            tmp[b] = ui[a];                        
            // extract a word
        }
        tmp[b] = '\0';
        words[w] = strdup(tmp);
        w++;
        if(ui[a] == '\0') break;
        a++; 
    }
    errorCode = interpreter(words, w);
    return errorCode;
}




