
#ifndef DIR_FUNCS_H
#define DIR_FUNCS_H

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Returns 1 if path is a directory
// 0 otherwise
int isDirectory(const char *path);

// Prints all files and directories and subdirectories
// found in a given path.
// NOTE: Path must start with ./
void printDirectory(const char *path);

// Searches path and all subdirectories for given filename
// Returns a pointer to a string containing the full path to
// the file from the main program
// NOTE: Path must start with ./
// THE RETURNED POINTER MUST BE FREED BY THE USER
char *findFile(const char *path, const char *filename);

#endif