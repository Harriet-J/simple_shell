#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        printf("#cisfun$ ");
        fflush(stdout); /* Ensure prompt is printed immediately */
        nread = getline(&line, &len, stdin);

        if (nread == -1)
        {
            free(line);
            exit(EXIT_SUCCESS);
        }

        /* Remove newline character from input */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        if (strcmp(line, "exit") == 0)
        {
            free(line);
            exit(EXIT_SUCCESS);
        }

        pid = fork();

        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            /* Child process */
            execlp(line, line, (char *)NULL);
            perror(line);
            exit(EXIT_FAILURE);
        }
        else
        {
            /* Parent process */
            waitpid(pid, &status, 0);
        }
    }
    return 0;
}


