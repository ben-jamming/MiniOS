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
#include <ctype.h>


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

void split_on_semicolon(char* str, char** first, char** second) {
    char* semicolon_ptr = strchr(str, ';');
    if (semicolon_ptr == NULL) {
        *first = strdup(str);
        *second = NULL;
    } else {
        int first_len = semicolon_ptr - str;
        *first = (char*)malloc(first_len + 1);
        if (*first == NULL) {
            fprintf(stderr, "Error: Out of memory\n");
            exit(1);
        }
        strncpy(*first, str, first_len);
        (*first)[first_len] = '\0';
        
        int second_len = strlen(str) - first_len - 1;
        *second = (char*)malloc(second_len + 1);
        if (*second == NULL) {
            fprintf(stderr, "Error: Out of memory\n");
            exit(1);
        }
        strncpy(*second, semicolon_ptr + 1, second_len);
        (*second)[second_len] = '\0';
    }
}

void writeToBackstore(PCB* pcb, char *filename) {
  int pid = pcb->pid;
  // Open the file in the current directory
  char filepath[PATH_MAX];
  snprintf(filepath, PATH_MAX, "./%s", filename);
  FILE *file = fopen(filepath, "r");

  if (file == NULL) {
    printf("Error: Could not open file %s\n", filename);
    return;
  }

  // Create the new file in the backing store directory
  char new_filepath[PATH_MAX];
  snprintf(new_filepath, PATH_MAX, "%s%s_%d", BACKINGSTORE_DIRECTORY, filename, pid);
  FILE *new_file = fopen(new_filepath, "w");

  if (new_file == NULL) {
    printf("Error: Could not create file %s_%d\n", filename, pid);
    return;
  }

  // Copy the contents of the current file to the new file
  char line[MAX_LINE_LENGTH];
  char left[MAX_LINE_LENGTH];
  char right[MAX_LINE_LENGTH];
  int fileSizeCounter = 0;
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) 
  {
    int hasSemicolon = 0;
    
    for (int j = 0; j < strlen(line); j++)
    {
      if (line[j] == ';')
      {
        hasSemicolon = 1;
        break;
      }
    }
    if (hasSemicolon == 1)
    {
      char* first;
      char* second;
      split_on_semicolon(line, &first, &second);
      fprintf(new_file, "%s\n%s", first, second);
      free(first);
      free(second);
      fileSizeCounter += 2;
    }
    else
    {
      fprintf(new_file, "%s", line);
      fileSizeCounter += 1;
    }
  }
  // Close the files
  fclose(file);
  fclose(new_file);


  pcb->fileName = strdup(new_filepath);
  pcb->fileSize = fileSizeCounter;


}
