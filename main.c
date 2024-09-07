#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1)
    {
        printf("#cisfun$ ");
        nread = getline(&line, &len, stdin);

        if (nread == -1)
        {
            free(line);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(line, "exit\n") == 0)
        {
            free(line);
            exit(EXIT_SUCCESS);
        }

        system(line);
    }
    return 0;
}

