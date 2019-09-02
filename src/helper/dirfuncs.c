
#include "dirfuncs.h"

// Returns 1 if path is a directory
// 0 otherwise
int isDirectory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

// Prints all files and directories and subdirectories
// found in a given path
void printDirectory(const char *path) {
    DIR *dir;
    struct dirent *ent;
    char *full_dir;

    dir = opendir(path);
    if(dir == 0) {
        return;
    }

    ent = readdir(dir);
    while(ent != 0) {
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            ent = readdir(dir);
            continue;
        }

        // full_dir points to a string containing the full directory path
        // e.g. test1/a/target instead of just target
        full_dir = malloc(strlen(ent->d_name) + strlen(path) + 2);

        strcpy(full_dir, path);
        strcat(full_dir, "/\0");
        strcat(full_dir, ent->d_name);

        printf("%s\n", full_dir + 2);

        if(isDirectory(full_dir)) {
            printDirectory(full_dir);
        }

        free(full_dir);

        ent = readdir(dir);
    }

    closedir(dir);

    return;
}

// Searches path and all subdirectories for given filename
// Returns a pointer to a string containing the full path to
// the file from the main program
// THE RETURNED POINTER MUST BE FREED BY THE USER
char *findFile(const char *path, const char *filename) {
    DIR *dir;
    struct dirent *ent;
    char *full_dir, *temp = 0;

    dir = opendir(path);
    if(dir == 0) {
        return 0;
    }

    ent = readdir(dir);
    while(ent != 0) {
        // ignore directories . and ..
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            ent = readdir(dir);
            continue;
        }

        // full_dir points to a string containing the full directory path
        // e.g. test1/a/target instead of just target
        full_dir = malloc(strlen(ent->d_name) + strlen(path) + 2);
        strcpy(full_dir, path);
        strcat(full_dir, "/\0");
        strcat(full_dir, ent->d_name);

        // check if this entry is the file
        if(strcmp(ent->d_name, filename) == 0) {
            // do not free full_dir if the file is found
            temp = full_dir;
            // remove leading "./"
            memmove(temp, (temp + 2), strlen(temp));
            break;
        }

        // check if this entry is a directory
        if(isDirectory(full_dir)) {
            temp = findFile(full_dir, filename);
            if(temp != 0) {
                free(full_dir);
                break;
            }
        }

        free(full_dir);

        ent = readdir(dir);
    }

    closedir(dir);

    return temp;
}