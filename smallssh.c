/*
Artem Kuryachy
CS 344 - Assignment 2 Winter 2022
Citations for the following program:
Date: 1/19/2022
tokenizer: https://edstem.org/us/courses/16718/discussion/1041791
getline() function reference: https://dev-notes.eu/2019/07/Get-a-line-from-stdin-in-C/
strncmp() function reference: https://www.tutorialspoint.com/c_standard_library/c_function_strncmp.htm
Last char reference: https://cboard.cprogramming.com/c-programming/371-how-do-i-get-last-character-string.html
getline() and strcmp() behavior: https://stackoverflow.com/questions/29397719/c-getline-and-strcmp-issue
getpid() function reference: https://man7.org/linux/man-pages/man2/getpid.2.html
exit() function reference: https://www.tutorialspoint.com/exit-vs-exit-in-c-cplusplus
*/
#include "smallssh.h"

// Current issue: input struct in header file non-accessible in main file 

int main(int argc, char *arg[]) {
	
	// Initialize main variables
	char *line = NULL;
	size_t len = 0;
	ssize_t lineSize = 0;
	char exstr[15] = "exit";
	int ret = 1;
	struct input *anInput = (struct input *)malloc(sizeof(struct input));

	// Run shell with user input 
	while (ret != 0) {

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
			printf("BLANK LINE IGNORE\n");
			fflush(stdout);
		}
		else if (strncmp(line, "cd", 2) == 0) {
			printf("Change Directory Command\n");
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

	return 0;
}