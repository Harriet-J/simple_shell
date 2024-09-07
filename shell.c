#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void execute_command(char *cmd)
{
    pid_t pid;
    char *argv[2];
    
    argv[0] = cmd;  /* Command to execute */
    argv[1] = NULL; /* Null-terminated array for execve */

    pid = fork();

    if (pid == 0) /* Child process */
    {
        execve(argv[0], argv, NULL);
        perror("./shell");
        exit(1);
    }
    else if (pid < 0) /* Fork failed */
    {
        perror("fork");
        exit(1);
    }
    else /* Parent process */
    {
        wait(NULL); /* Wait for child process to finish */
    }
}


