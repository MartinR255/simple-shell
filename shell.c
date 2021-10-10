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
	int input_length = 50;
	char input_command[input_length];	
	char *divided_command[10];

	//extra variables needed in 
	int process, character; //character variable is used in flushing stdin
	int position = 0;
	char *argument;
	char *copy_of_input_command, *copy_of_last_command;
	
	while (1) {
		//alocete memory to make copy of previous command in case ! is called as command
		//char *copy_of_last_command = (char *) malloc(strlen(input_command) + 1); 
		//strcpy(copy_of_last_command, input_command);
		copy_of_last_command = strdup(input_command);
		
		printf("premia> ");
		scanf("%[^\n]", input_command); //fgets(input_command, 50, stdin);
		
		//clears stdin so it won't be stuck in infinite loop
		//fflush(stdin) might also work but not in this case
		while ((character = getchar()) != '\n' && character != EOF) { }

		if (strcmp(input_command, "exit") == 0) {
			//free(copy_of_last_command);
			exit(1);
		} 
		else if (strcmp(input_command, "!") == 0) {
			strcpy(input_command, copy_of_last_command);
		} 
		//free(copy_of_last_command);

		if (shell_env_variable_set) { //if env variable SHELL_LOG is set, write commands into SHELL_LOG file
			fd = fopen(file_name, "a+");
			fprintf(fd, "%s\n", input_command);
			fclose(fd);
		} 
		//execute command if env variable SHELL_LOG is not set
		//making copy of input comamnd to be manipulated iso the original can be used if ! is called as next command 
		//char *copy_of_input_command = (char *) malloc(strlen(input_command) + 1); 
		//strcpy(copy_of_input_command, input_command);
		copy_of_input_command = strdup(input_command);

		//divides the input into separate arguments
		position = 1;
		argument = strtok(copy_of_input_command, " ");
		divided_command[0] = argument;	
		while ( argument != NULL ) {
			argument = strtok(NULL, " ");
			divided_command[position] = argument; 
			position++;
		}

		//creating new process
		process = fork();
		if ( process < 0 ) {
			fprintf(stderr, "Fork failed\n");
			exit(1);
		} 
		else if ( process == 0 ) {
			execvp(divided_command[0], divided_command);
			printf("Invalid command\n");
		} 
		wait(NULL);
		//free(copy_of_input_command); //free copy that have been manipulated above
	}
	return 0;
}