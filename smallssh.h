#pragma once
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

struct input {

	char *command;

};


void exitprogram() {
	// Kill all ongoing processess prior to exit
    kill(-1, SIGKILL);
}


struct input *tokenize(char *currLine)
{
    // Allocate pointer space
    struct input *currComm = malloc(sizeof(struct input));

    // For use with strtok_r
    char *saveptr = NULL;

    char *token = strtok_r(currLine, " ", &saveptr);
    currComm->command = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currComm->command, token);

    printf("Struct working: %s\n", currComm->command);

    return currComm;
}