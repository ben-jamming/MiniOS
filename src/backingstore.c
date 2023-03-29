#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include "shell.h"
#include "backingstore.h"

// Initialize backingstore file
int init_backing_store() {
    // Create backingstore file
    const char *filename = "backingstore.txt";
    FILE *fp;
    int exist;
    int errCode = 0;

    // Check if file exists already
    if ((fp = fopen(filename, "r")) != NULL) {
        exist = 1;
        fclose(fp);
    } else {
        exist = 0;
    }

    // If file exists, delete it
    if (exist) {
        remove(filename);
    }

    // Create an empty file
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error creating file!\n");
        errCode = 1;
    }
    fclose(fp);
    printf("File created successfully.\n");
    return errCode;
}

// Delete backingstore file upon quit
int remove_backing_store(){
    const char *filename = "backingstore.txt";
    int errCode = 0;
    if (remove(filename) != 0) {
        printf("Error deleting file!\n");
        errCode = 1;
    }
    printf("File deleted successfully.\n");
    return errCode;
}