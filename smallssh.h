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
#include <sys/wait.h>

struct input {
	char *command;
};

void exitprogram() {
    exit(0);
}

void user_input() {

	struct input *dirchange(char *currLine);

	// Initialize main variables
	char *line = NULL;
	size_t len = 0;
	ssize_t lineSize = 0;
	char exstr[15] = "exit";
	int ret = 1;
	struct input *anInput = (struct input *)malloc(sizeof(struct input));

	// Run shell with user input 
	while (ret != 0) {
		// Declare functions to be used
struct input *tokenize(char *currLine);

// Prompt user, get input
printf(": ");
fflush(stdout);
lineSize = getline(&line, &len, stdin);
line[strcspn(line, "\r\n")] = 0;

// Ignore comments
if (line[0] == '#') {
	printf("COMMENT IGNORE\n");
	fflush(stdout);
}
// Ignore blank lines
else if (line[0] == ' ') {
	fflush(stdout);
}
else if (strncmp(line, "cd", 2) == 0) {
	printf("Change Directory Command\n");
	dirchange(line);
	fflush(stdout);
}
else if (strcmp(line, "status") == 0) {
	printf("Status Command\n");
	statusization();
	fflush(stdout);
}
else if (line[(strlen(line) - 1)] == '&') {
	printf("BACKGROUND PROCESS\n");
	fflush(stdout);
}
else if (strstr(line, "$$")) {
	int pid = getpid();
	printf("%d\n", pid);
	fflush(stdout);
}
else {
	tokenize(line);
	printf("Command entered: %s\n", anInput->command);
	ret = strncmp(line, exstr, 2048);
	fflush(stdout);
}
	}
	free(line);

	if (ret == 0) {
		exitprogram();
	}
}

void statusization() {
	int status;

	if (WIFEXITED(status)) {
		printf("exit value %d\n", WEXITSTATUS(status));
		fflush(stdout);
	}
	else if (WIFSIGNALED(status)) {
		printf("terminating signal %d\n", WTERMSIG(status));
		fflush(stdout);
	}
}

struct input *dirchange(char *currLine) {

	char newdir[2048];
	char *usrpath;
	getcwd(newdir, sizeof(newdir));
	int success;
	size_t allocSize = sizeof(char) * 2048;
	usrpath = (char *)malloc(allocSize);
	char *ptr;

	if (currLine[2] == NULL) {
		success = chdir(getenv("HOME"));

		if (success == 0) {
			getcwd(newdir, sizeof(newdir));
			printf("Smallssh Working Directory change to %s\n", newdir);
			fflush(stdout);
		}
	}
	else {
		usrpath = strstr(currLine, " ");

		// Check for relative or absolute path
		if (usrpath[0] != '/') {
			
			// Relative
			fflush(stdout);
			usrpath++;

			if ((ptr = getcwd(newdir, allocSize)) == NULL) {
				perror("getcwd() error");
				fflush(stdout);
			}
			else {
				strcat(ptr, "/");
				strcat(ptr, usrpath);
				success = chdir(ptr);

				if (success == 0) {
					printf("Smallsh Working Directory changed to %s\n", ptr);
					fflush(stdout);
				}
				else {
					printf("Directory not available or not found\n");
					fflush(stdout);
				}
			}
		}
		else {
			// Skip space
			usrpath++;
			success = chdir(usrpath);

			if (success == 0) {
				if ((ptr = getcwd(usrpath, allocSize)) == NULL) {
					perror("getcwd() error");
					fflush(stdout);
				}
				else {
					printf("Smallsh Working Directory changed to %s\n", ptr);
					fflush(stdout);
				}
			}
			else {
				printf("Directory not available or not found\n");
				fflush(stdout);
			}

		}
	}

	fflush(stdout);
	return 0;
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