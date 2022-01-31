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
	char *t_command;
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
	struct input *varexpanse(char *currLine);
	char *tokenize(struct input *currLine);
	int step5_commands();

	// Initialize variables
	size_t len = 0;
	ssize_t lineSize = 0;
	char *line = NULL;
	char exstr[15] = "exit";
	char **args;
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
		// Evoke Expansion of Variable to get PID
		else if (strstr(line, "$$")) {
			// Pass user input string to input struct and varexpanse() function
			anInput->command = calloc(strlen(line) + 1, sizeof(char));
			strcpy(anInput->command, line);
			varexpanse(line);
		}
		// Evoke handling as background process
		else if (line[(strlen(line) - 1)] == '&') {
			printf("BACKGROUND PROCESS\n");
			fflush(stdout);
		}
		// Evoke exit or non-built-in commands
		else {
			// Ret returns '0' upon user input of 'exit'
			ret = strncmp(line, exstr, 2048);
			args = tokenize(line);
			
			if (args) {
				anInput->t_command = calloc(strlen(args) + 1, sizeof(char));
				strcpy(anInput->t_command, args);
				step5_commands();
			}
			else {
				printf("Command Not Recognized\n");
				fflush(stdout);
			}			
		}
	}

	free(line);
	// Exit function 'exit()' upon user input 
	if (ret == 0) {
		exitprogram();
	}
}

// Provide user with either the exit status or terminating signal of the last foreground process ran by the shell
int statusization() {

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

	return 0;
}

// Change current working directory
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

// Expansion of Variable functionality
struct input *varexpanse(char *currLine) {

	char *aug_str(char *str, char *orig, char *rep);

	char *exp = "$$";
	int pid = getpid();
	char *res;
	char pid_str[2048];

	// Transform pid to string
	sprintf(pid_str, "%d", pid); // print int 'n' into the char[] buffer 

	// Swap all instances of 'exp' variable in user input string
	res = aug_str(currLine, exp, pid_str);

	// Provide augmented user input as output
	printf("%s\n", res);
}

char *aug_str(char *str, char *orig, char *rep) {

	// Initialize variables and types
	char buff[2048] = { 0 };
	char *insertion = &buff[0];
	const char *temp = str;
	size_t ndle_len = strlen(orig);
	size_t repl_len = strlen(rep);

	// Iterate infinitely until break condition met
	while (1) {
		// Initialize position tracker pointer
		const char *p = strstr(temp, orig);

		// Needle traversal concluded; copy remaining portion
		if (p == NULL) {
			strcpy(insertion, temp);
			break;
		}

		// Retain part before $$
		memcpy(insertion, temp, p - temp);
		insertion += p - temp;
		// Copy replacement string PID i.e. perform swap for expansion of variables
		memcpy(insertion, rep, repl_len);
		insertion += repl_len;
		// Advance and adjust for new position, moving on
		temp = p + ndle_len;
	}
	return buff;
}

char *tokenize(struct input *currLine)
{
	// Initialize variables
	char *str;
	char *found;
	char *saveptr = NULL;
	int counter = 0;

	// Duplicate string to preserve original
	str = strdup(currLine);
	// Allocate storage	
	currLine->t_command = calloc(strlen(str) + 1, sizeof(char));

	// Slice through input string duplicate with given delimiters
	while ((found = strsep(&str, "  \0\t\r\n\a")) != NULL) {

		// Output redirection
		if (strcmp(found, ">") == 0) {
			printf("Output Redirect Detected \n");
		}

		// Input redirection
		if (strcmp(found, "<") == 0) {
			printf("Input Redirect Detected \n");
		}

		// Background process

		// Build t_command parameter of input struct
		if (counter == 0) {
			strcpy(currLine->t_command, found);
			counter++;
		}
		else {
			strcat(currLine->t_command, found);
		}
	}
	return currLine->t_command;
}


// Deploy commands designated as non-built-in i.e. those described in Step 5
int step5_commands() {

	char *newargv[] = { "/bin/ls", "-a", NULL };
	int childStatus;

	// Fork a new process
	pid_t spawnPid = fork();

	switch (spawnPid) {
	case -1:
		perror("fork()\n");
		exit(1);
		break;

	case 0:
		// In the child process
		printf("CHILD(%d) running ls command\n", getpid());
		// Replace the current program with "/bin/ls"
		execv(newargv[0], newargv);
		// exec only returns if there is an error
		perror("execve");
		exit(2);
		break;

	default:
		// In the parent process
		// Wait for child's termination
		spawnPid = waitpid(spawnPid, &childStatus, 0);
		printf("PARENT(%d): child(%d) terminated. Exiting\n", getpid(), spawnPid);
		exit(0);
		break;
	}
	return 0;
}