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
conflicting types error: https://stackoverflow.com/questions/5691650/conflicting-types-error-when-compiling-c-program-using-gcc
skipping spaces: https://stackoverflow.com/questions/4295754/how-to-remove-first-character-from-c-string
getcwd() function reference: https://stackoverflow.com/questions/9697056/printing-present-working-directory-in-c-using-pointers
chrdir() function reference: https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
statusization() function reference: https://stackoverflow.com/questions/39913885/waitpid-in-which-cases-both-wifexited-and-wifsignaled-will-be-false
Expansion of Variables handling: https://javatutoring.com/c-program-replace-occurrences-of-a-character-with-string/
replacing substrings: https://stackoverflow.com/questions/32413667/replace-all-occurrences-of-a-substring-in-a-string-in-c
delimiter set: https://stackoverflow.com/questions/38557481/what-does-t-r-n-a-stand-for
tokenization: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
strsep() function reference: https://c-for-dummies.com/blog/?p=1769
storing into a struct array: https://cboard.cprogramming.com/c-programming/71241-storing-into-array-struct.html
accessing struct variables outside of assigning function: https://stackoverflow.com/questions/26639036/accessing-struct-variables-outside-of-function-in-c
*/
#include "smallssh.h"

// Current issue: input struct in header file non-accessible in main file 

int main(int argc, char *arg[]) {
	

	void user_input();
	user_input();

	return 0;
}

