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
#include <fcntl.h>

// Globally initialized variables and arrays for I/O redirection - passing back and forth from struct seem unnecessarily complex at the moment
int red_inp = 0;
int red_out = 0;
char *usrinp;
char *usrout;

struct input {
	char *command;
	char *tokens[512][2048];
} theInput;

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
	char **tokenize(struct input *myInput, const char *theComm);
	int step5_commands(char **args);

	// Initialize variables
	size_t len = 0;
	ssize_t lineSize = 0;
	char *line = NULL;
	char exstr[15] = "exit";
	int ret = 1;
	struct input *anInput = (struct input *)malloc(sizeof(struct input));
	char **args;

	// Run shell while user has not inputed 'exit'
	while (ret != 0) {

		memset(anInput->tokens, NULL, sizeof(anInput->tokens[0][0])*512*2048);

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
			ret = strncmp(line, exstr, 15);

			// Tokenize command into null-terminated array of tokens, pass this to step5_commands() which evokes fork()
			args = tokenize(&theInput, line);
			step5_commands(args);

		}
		usrinp = NULL;
		usrout = NULL;
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
			printf("Smallssh Working Directory changed to %s\n", newdir);
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

char **tokenize(struct input *myInput, const char *theComm) {
	
	// Initialize variables
	char *token;
	int index = 0;
	red_inp = 0;
	red_out = 0;

	// Allocate space for pointer to array of pointers
	char **tokens = malloc(512 * sizeof(char *));

	// Begin tokenization
	token = strtok(theComm, " \0\t\r\n\a");

	while (token != NULL) {

		// Verify redirection
		// Input
		if (strcmp(token, "<") == 0) {
			// Set redirection input trigger
			red_inp = 1;
			// Assign user-specified file name to usrinp to act input file
			usrinp = strtok(NULL, " \0\t\r\n\a");
			// Tokenize remaining arguments
			token = strtok(NULL, " \0\t\r\n\a");
			// Null terminate at current index and increment index
			tokens[index] = NULL;
			index++;
		}
		// Output - functionally identical to Input block
		else if (strcmp(token, ">") == 0) {
			red_out = 1;
			usrout = strtok(NULL, " \0\t\r\n\a");
			token = strtok(NULL, " \0\t\r\n\a");
			tokens[index] = NULL;
			index++;
		}
		else {
			// Add token to array at index
			tokens[index] = token;
			// Add token to struct array, just in case
			strcpy(myInput->tokens[index], token);
			// Continue tokenization
			index++;
			token = strtok(NULL, " \0\t\r\n\a");
		}
	}
	// NULL terminate to comply with execvp()
	tokens[index] = NULL;

	return tokens;
}

int step5_commands(char **args) {

	int childStatus;

	// Fork a new process
	pid_t spawnPid = fork();

	switch (spawnPid) {

		// Erroneous forking
		case -1:
			perror("fork()\n");
			exit(1);
			break;

		// Child process execution
		case 0:

			// Input Redirection 
			if (red_inp) {

				// Open user-specified input file for reading only
				int fd0 = open(usrinp, O_RDONLY);

				// Notify user if open() unsuccessful, set exit status to 1
				if (fd0 == -1) {
					printf("Unable to open input file ' %s ' for reading-only.\n", usrinp);
					fflush(stdout);
					exit(1);
				}
				// Successful opening - use dup2() w/ stdin to redirect
				else {
					if (dup2(fd0, STDIN_FILENO) == -1) {
						perror("dup2");
					}
					// Close file
					close(fd0);
				}

			}

			// Output Redirection
			if (red_out) {
				// Need to check if file exits, 
				// If it does open it, truncate it, and close it
				// If not do the below
				// Create user-specified output file for writing only
				int fd1 = creat(usrout, 0644);
				
				// Notify user if shell cannot access output file
				if (fd1 == -1) {
					printf("Unable to create ' %s ' for output\n");
					fflush(stdout);
					exit(1);
				}
				// Successful creation - use dup2() w/ stdout to redirect
				else {
					if (dup2(fd1, STDOUT_FILENO) == -1) {
						perror("dup2");
					}
					// Close file
					close(fd1);
				}
			}

			// Replace the current program with user command
			execvp(args[0], args);
			// exec only returns if there is an error
			perror("execvp");
			exit(2);
			break;

		// Parent process 
		default:
			// Wait for child's termination
			spawnPid = waitpid(spawnPid, &childStatus, 0);

			if (spawnPid == -1) {
				perror("waitpid");
				exit(1);
			}

			break;
	}

	return 0;
}