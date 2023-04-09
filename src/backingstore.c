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
#include <limits.h>


#define PATH_MAX 4096
#define MAX_LINE_LENGTH 1000
#define BACKINGSTORE_DIRECTORY "./backingstore/"

// Delete backingstore folder upon quit
int removeBackingStore(){
    const char *foldername = "backingstore";
    int errCode = 0;
    char command[100];
    snprintf(command, sizeof(command), "rm -rf %s/*", foldername);
    system(command);
    if (rmdir(foldername) != 0) {
        perror("Error deleting backingstore folder");
        errCode = -1;
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
    mkdir(foldername, 0700);
    return errCode;
}
void testWriteToBackstore(int pid, char *filename) {
    // Construct the expected file path and backing store file path
    char expected_file_path[PATH_MAX];
    snprintf(expected_file_path, PATH_MAX, "./%s", filename);
    char backing_store_file_path[PATH_MAX];
    snprintf(backing_store_file_path, PATH_MAX, "%s%s_%d", BACKINGSTORE_DIRECTORY, filename, pid);

    // Read the contents of the original file
    char *original_contents = NULL;
    long original_size = 0;
    FILE *original_file = fopen(expected_file_path, "rb");
    if (original_file) {
        fseek(original_file, 0, SEEK_END);
        original_size = ftell(original_file);
        rewind(original_file);

        original_contents = malloc(original_size);
        if (original_contents) {
            fread(original_contents, 1, original_size, original_file);
        }

        fclose(original_file);
    }

    // Read the contents of the backing store file
    char *backing_store_contents = NULL;
    long backing_store_size = 0;
    FILE *backing_store_file = fopen(backing_store_file_path, "rb");
    if (backing_store_file) {
        fseek(backing_store_file, 0, SEEK_END);
        backing_store_size = ftell(backing_store_file);
        rewind(backing_store_file);

        backing_store_contents = malloc(backing_store_size);
        if (backing_store_contents) {
            fread(backing_store_contents, 1, backing_store_size, backing_store_file);
        }

        fclose(backing_store_file);
    }

    // Ensure the two files are the same
    if (original_size != backing_store_size || memcmp(original_contents, backing_store_contents, original_size) != 0) {
        printf("Error: The backing store file does not match the original file\n");
        return;
    }

    printf("Test passed: The backing store file matches the original file\n");
}

void writeToBackstore(int pid, char *filename) {
  // Open the file in the current directory
  char filepath[PATH_MAX];
  snprintf(filepath, PATH_MAX, "./%s", filename);
  FILE *file = fopen(filepath, "r");

  if (file == NULL) {
    printf("Error: Could not open file %s\n", filename);
    return;
  }

  // Create the new file in the backing store directory
  char new_filename[PATH_MAX];
  snprintf(new_filename, PATH_MAX, "%s_%d", filename, pid);
  char new_filepath[PATH_MAX];
  snprintf(new_filepath, PATH_MAX, "%s%s", BACKINGSTORE_DIRECTORY, new_filename);
  FILE *new_file = fopen(new_filepath, "w");

  if (new_file == NULL) {
    printf("Error: Could not create file %s\n", new_filename);
    return;
  }

  // Copy the contents of the current file to the new file
  char line[MAX_LINE_LENGTH];
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    char *semicolon_ptr = strchr(line, ';');
    if (semicolon_ptr != NULL) {
      // Split the line on the semicolon and write both lines to the new file
      *semicolon_ptr = '\0'; // Replace the semicolon with a null terminator
      fprintf(new_file, "%s\n", line);
      fprintf(new_file, "%s", semicolon_ptr + 1); // Move past the semicolon
    } else {
      // Write the entire line to the new file
      fprintf(new_file, "%s", line);
    }
  }
  //printf("Wrote file %s to backing store\n", new_filename);
  // Close the files
  fclose(file);
  fclose(new_file);

 //testWriteToBackstore(pid, filename);
}





