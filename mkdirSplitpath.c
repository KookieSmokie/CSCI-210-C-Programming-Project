#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

/* Splits pathName into dirName and baseName, and returns the node for the
 * parent directory of baseName (NULL on error). */
struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    char* lastSlash = strrchr(pathName, '/');

    if (lastSlash == NULL) {
        /* just a name: relative to the current directory */
        strcpy(dirName, "");
        strcpy(baseName, pathName);
    } else if (lastSlash == pathName) {
        /* "/" or "/name": parent is root */
        strcpy(dirName, "/");
        strcpy(baseName, lastSlash + 1);
    } else {
        size_t len = lastSlash - pathName;
        strncpy(dirName, pathName, len);
        dirName[len] = '\0';
        strcpy(baseName, lastSlash + 1);
    }

    /* choose the starting point */
    struct NODE* curr = (pathName[0] == '/') ? root : cwd;

    /* walk the directory portion, one component at a time */
    char temp[strlen(dirName) + 1];
    strcpy(temp, dirName);

    char* token = strtok(temp, "/");
    while (token != NULL) {
        struct NODE* child = curr->childPtr;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0 && child->fileType == 'D')
                break;
            child = child->siblingPtr;
        }
        if (child == NULL) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        curr = child;
        token = strtok(NULL, "/");
    }

    return curr;
}

void mkdir(char pathName[]) {
    char baseName[64];
    char dirName[strlen(pathName) + 1];

    struct NODE* parent = splitPath(pathName, baseName, dirName);
    if (parent == NULL) {
        return;  /* splitPath already printed the error */
    }

    if (strcmp(baseName, "") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    /* check for an existing child with the same name */
    struct NODE* child = parent->childPtr;
    struct NODE* last = NULL;
    while (child != NULL) {
        if (strcmp(child->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        last = child;
        child = child->siblingPtr;
    }

    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parent;

    if (last == NULL) {
        parent->childPtr = newNode;   /* first child */
    } else {
        last->siblingPtr = newNode;   /* append after the last sibling */
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}