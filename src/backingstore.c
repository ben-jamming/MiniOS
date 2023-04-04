#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include "shell.h"
#include "backingstore.h"

// Delete backingstore folder upon quit
int removeBackingStore(){
    const char *foldername = "backingstore";
    int errCode = 0;
    if (rmdir(foldername) != 0) {
        printf("Error deleting folder!\n");
        errCode = 1;
    } else {
        printf("Folder deleted successfully.\n");
    }
    return errCode;
}

// Initialize backingstore folder
int initBackingStore() {
    // Create backingstore folder
    const char *foldername = "backingstore";
    int errCode = 0;

    // Check if folder exists already
    struct stat st = {0};
    if (stat(foldername, &st) != -1) {
        removeBackingStore();
    } 
    // If folder doesn't exist, create it
    if (mkdir(foldername, 0700) == -1) {
        printf("Error creating folder!\n");
        errCode = 1;
    } else {
        printf("Folder created successfully.\n");
    }

    return errCode;
}






