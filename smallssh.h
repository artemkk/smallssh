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

// Input struct for storing and handling user command
struct input {
	char *command;
};

// Exit function
void exitprogram() {
    // Terminates child process prior to terminating parent
	// Returns exit status zero upon success
	exit(0);
}

// Ask user for input, assess input and conditionally execute desired functionality
void user_input() {

	// Initialize functions
	struct input *dirchange(char *currLine);
	struct input *tokenize(char *currLine);

	// Initialize variables
	size_t len = 0;
	ssize_t lineSize = 0;
	char *line = NULL;
	char exstr[15] = "exit";
	int ret = 1;
	struct input *anInput = (struct input *)malloc(sizeof(struct input));

	// Run shell while user has not inputed 'exit'
	while (ret != 0) {

		// Prompt user, get input
		printf(": ");
		fflush(stdout);

		// Receive input at memory address of 'line' pointer
		lineSize = getline(&line, &len, stdin);
		// Reformat input for handling
		line[strcspn(line, "\r\n")] = 0;

		// Check to ignore comments
		if (line[0] == '#') {
			continue;
		}
		// Check to ignore blank lines
		else if (line[0] == ' ') {
			continue;
		}
		// Evoke change of directory dirchange() function
		else if (strncmp(line, "cd", 2) == 0) {
			dirchange(line);
		}
		// Evoke status output statusization() function
		else if (strcmp(line, "status") == 0) {
			statusization();
		}
		// Evoke handling as background process
		else if (line[(strlen(line) - 1)] == '&') {
			printf("BACKGROUND PROCESS\n");
			fflush(stdout);
		}
		// Evoke Expansion of Variable to get PID
		else if (strstr(line, "$$")) {
			int pid = getpid();
			printf("%d\n", pid);
			fflush(stdout);
		}
		// Evoke exit or non-built-in commands
		else {
			tokenize(line);
			printf("Command entered: %s\n", anInput->command);
			// Ret returns '0' upon user input of 'exit'
			ret = strncmp(line, exstr, 2048);
			fflush(stdout);
		}
	}

	free(line);
	// Exit function 'exit()' upon user input 
	if (ret == 0) {
		exitprogram();
	}
}

// Provide user with either the exit status or terminating signal of the last foreground process ran by the shell
void statusization() {

	int status;

	// Provide Exit Status 
	if (WIFEXITED(status)) {
		printf("Exit Status Value: %d\n", WEXITSTATUS(status));
		fflush(stdout);
	}
	// Provide Terminating Signal
	else if (WIFSIGNALED(status)) {
		printf("Terminating Signal: %d\n", WTERMSIG(status));
		fflush(stdout);
	}
}

struct input *dirchange(char *currLine) {

	// Initialize variables
	char *ptr;
	char *usrpath;
	int success;
	char newdir[2048];
	
	// Allocate space
	size_t allocSize = sizeof(char) * 2048;
	usrpath = (char *)malloc(allocSize);

	//Get Current Working Directory into 'newdir'
	getcwd(newdir, sizeof(newdir));

	// Check if input is 'cd' only or 'cd <directory name>'
	if (currLine[2] == NULL) {
		// 'cd' only

		//Set CWD to current "HOME" path directory
		success = chdir(getenv("HOME"));

		// Verify successful switch
		if (success == 0) {
			getcwd(newdir, sizeof(newdir));
			printf("Smallssh Working Directory change to %s\n", newdir);
			fflush(stdout);
		}
	}
	else {
		// 'cd <directory name>'
		
		//Reformat usrer input to exclude 'cd'
		usrpath = strstr(currLine, " ");

		// Check for relative or absolute path
		if (usrpath[0] != '/') {
			
			// Relative
			// Reformat user input to exclude leading space character
			usrpath++;

			// Retrieve current working directory
			if ((ptr = getcwd(newdir, allocSize)) == NULL) {
				perror("getcwd() error");
				fflush(stdout);
			}
			// If successful, modify CWD to include relative path provide by user
			else {

				strcat(ptr, "/");
				strcat(ptr, usrpath);
				success = chdir(ptr);

				// Verify successful switch
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
			// Absolute
			// Reformat user input to exclude leading space character
			usrpath++;

			// Switch CWD to absolute path provided by user
			success = chdir(usrpath);

			// Verify successful switch
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