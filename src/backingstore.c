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
    rmdir(foldername);
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
    mkdir(foldername, 0700);
    return errCode;
}



