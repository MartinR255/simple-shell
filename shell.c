#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main (int argc, char *argv[]) {
	//preparation in case SHELL_LOG env variable is set before start
	unsigned int shell_env_variable_set = 0;
	FILE *fd;
	char *file_name = getenv("SHELL_LOG");
	if (file_name != NULL) {
		shell_env_variable_set = 1;
	}

	//variables for executable command
	char input_command[50];	
	char *divided_command[10];

	//extra variables needed in 
	int process = 0, position = 1, character = 0; //character variable is used in flushing stdin
	char *argument = NULL, *copy_of_last_command = NULL;
	
	while (1) {
		printf("premia> ");
		scanf("%[^\n]", input_command); //fgets(input_command, 50, stdin);
		
		//clears stdin so it won't be stuck in infinite loop
		//fflush(stdin) might also work but not in this case
		while ((character = getchar()) != '\n' && character != EOF) { }

		if (strcmp(input_command, "exit") == 0) {
			exit(1);
		} 
		else if (strcmp(input_command, "!") == 0) {
			strcpy(input_command, copy_of_last_command);
		} 

		if (shell_env_variable_set) { //if env variable SHELL_LOG is set, write commands into SHELL_LOG file
			fd = fopen(file_name, "a+");
			fprintf(fd, "%s\n", input_command);
			fclose(fd);
		}

		//creating new process
		process = fork();
		if ( process < 0 ) {
			fprintf(stderr, "Fork failed\n");
			exit(1);
		} 
		else if ( process == 0 ) {
  	                argument = strtok(input_command, " ");
                	divided_command[0] = argument;
			while ( argument != NULL ) {
                        	argument = strtok(NULL, " "); 
				divided_command[position] = argument;
                        	position++;
			}


			execvp(divided_command[0], divided_command);
			printf("Invalid command\n");
			exit(1);
		} 
		wait(NULL);
		copy_of_last_command = strdup(input_command);
	}
	return 0;
}
