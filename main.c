#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1)
    {
        printf("#cisfun$ ");
        fflush(stdout);
        read = getline(&line, &len, stdin);

        if (read == -1) /* Handle end of file (Ctrl+D) */
        {
            free(line);
            exit(0);
        }

        if (line[read - 1] == '\n')
            line[read - 1] = '\0'; /* Remove newline character */

        execute_command(line);
    }
}

