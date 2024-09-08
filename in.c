#include "shell.h"

/**
 * inputvalidator - validates input buffer
 * @fd: file descriptor
 * @buf: buffer
 *
 * Return: return value of command, 2 for syntax errors,
 * or negative numbers for syscall errors
 *
 */
int inputvalidator(char **buf, int fd)
{
	char *newbuf, *bufgl, *bufptr = *buf;
	ssize_t lenr;
	size_t lenbuf;
	int start = 1;
	int complete = 0;

	linecount(1);
	if (*bufptr == 0)
		return (0);
	while (*bufptr)
	{
#ifdef DEBUGVALID
		printf("Invalid loop complete:%d:bufptr:%s", complete, bufptr);
#endif
		while ((*bufptr == ' ' || *bufptr == '\n') && !(complete & 3))
			bufptr++;
		if (*bufptr == 0)
			break;
		if (start)
		{
			if (*bufptr == ';')
			{
				free(*buf);
				if (*(bufptr + 1) == ';')
					printerr(": Syntax Error \";;\" unexpected\n");
				else
					printerr(": Syntax Error \";\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
			if (*bufptr == '&')
			{
				free(*buf);
				if (*(bufptr + 1) == '&')
					printerr(": Syntax error \"&&\" unexpected\n");
				else
					printerr(": Syntax error \"&\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
			if (*bufptr == '|')
			{
				free(*buf);
				if (*(bufptr + 1) == '|')
					printerr(": syntax error \"||\" unexpected\n");
				else
					printerr(":syntax error \"|\" enexpected\n");
				setsvar("0", "2");
				return (2);
			}
			start = 0;
		}
		if (bufptr[0] == '\n' && bufptr[1] == 0)
			break;
		if (*bufptr == '#' && !(complete & 3) &&
				(bufptr == *buf || *(bufptr - 1) == ' '))
		{
			*bufptr = 0;
			break;
		}
		complete &= ~4;
#ifdef DEBUFVALID
		printf("!(complete & 3): %d\n", !(complete & 3));
#endif
		if (*bufptr == '"' && !(complete & 3))
		{
			complete |= 2;
			bufptr++;
			continue;
		}
		if (*bufptr == '"' && complete & 2)
			complete &= ~2;
		if (*bufptr == '\'' && !(complete & 3))
		{
			complete |= 1;
			bufptr++;
			continue;
		}
		if (*bufptr == '\'' && complete & 1)
			complete &= ~1;
		if (bufptr[0] == '&' && !(complete & 3))
		{
			if (bufptr[1] == '&')
			{
				complete |= 4;
				start = 1;
				bufptr++;
			}
			else if (bufptr[1] == '|')
			{
				free(*buf);
				printerr(": syntax error \"|\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
			else if (bufptr[1] == ';')
			{
				free(*buf);
				printerr(": syntax error \";\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
		}
		if ((bufptr[0] == '|') && !(complete & 3))
		{
			if (bufptr[1] == '|')
			{
				complete |= 4;
				start = 1;
				bufptr++;
			}
			else if (bufptr[1] == ';')
			{
				free(*buf);
				printerr(": syntax error \";\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
			else if (bufptr[1] == '&')
			{
				free(*buf);
				printerr(": syntax error \"&\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
		}
		if (*bufptr == ';')
		{
			if(*(bufptr + 1) == ';')
			{
				free(*buf);
				printerr(": syntax error \";;\" unexpected\n");
				setsvar("0", "2");
				return (2);
			}
			start = 1;
		}
		bufptr++;
	}
#ifdef DEBUGVALID
	printf("out of while complete:%d\n", complete);
#endif
	if (complete & 7)
	{
#ifdef DEBUFVALID
		printf("not complete:%d", complete);
#endif
		bufgl = NULL;
		if (isatty(fd))
			fprintstrs(1, ">", NULL);
		lenr = _getline(&bufgl, fd);
		if (lenr == 0 && !isatty(fd))
		{
			free(buf);
			free(bufgl);
			printerr(": syntax error: unterminated quoated string\n");
			return (-1);
		}
		if (lenr == -1)
		{
			;
		}
		lenbuf = _strlen(*buf);
		newbuf = malloc(lenbuf + lenr + 1);
		_strcpy(newbuf, *buf);
		_strcpy(newbuf + lenbuf, bufgl);
		free(*buf);
		free(bufgl);
#ifdef DEBUGVALID
		printf("passing buf:%s\n", newbuf);
#endif
		return (inputvalidator(&newbuf, fd));
	}
#ifdef DEBUGVALID
	printf("Final buf:%s\n", *buf);
#endif
	sethist(*buf);
	return (parseargs(buf));
}

/**
 * shintmode - shell interactive mode
 * Return: 0
 */
int shintmode(void)
{
	char *bufgl = NULL, *pwd;
	ssize_t lenr = 0, eofflag =0, ret = 0;
	int istty = isatty(0) && isatty(1);

	while(!eofflag)
	{
#ifdef DEBUGMODE
		printf("At terminal prompt\n");
#endif
		if (istty)
		{
			pwd = _getenv("PWD");
			if (pwd != NULL)
			{
				fprintstrs(1, "Simple shell:", pwd, "$ ", NULL);
				free(pwd);
			}
			else
			{
				fprintstrs(1, "Simple shell$", NULL);
			}
		}
		lenr = _getline(&bufgl, STDIN_FILENO);
		if (lenr == 0 || lenr == -1)
		{
			free(bufgl);
			break;
		}
		if (bufgl[lenr - 1] != '\n')
			eofflag = 1;
#ifdef DEBUGMODE
		printf("calling parseargs %s\n", bufgl);
#endif
		ret = inputvalidator(&bufgl, STDIN_FILENO);
		bufgl = NULL;
		if (eofflag)
			break;
	}
	return (ret);
}

/**
 * scriptmode - shell script mode
 * @av: arguments
 *
 * Return: 0 (success), -1 (failure)
 */
int scriptmode(char *av[])
{
	char *bufgl = NULL;
	int infile;
	ssize_t lenr = 0, eofflag = 0, ret = 0;

	infile = open(av[1], O_RDONLY);
	if (infile == -1)
	{
		fprintstrs(STDERR_FILENO, av[0], ": 0: can't open ", av[1], "\n", NULL);
		return (127);
	}
	while (!eofflag)
	{
		lenr = _getline(&bufgl, infile);
		if (lenr == 0 || lenr == -1)
		{
			free(bufgl);
			break;
		}
		if (bufgl[lenr - 1] != '\n')
			eofflag = 1;
#ifdef DEBUGMODE
		print("calling parseargs %s\n", bufgl);
#endif
		ret = inputvalidator(&bufgl, STDIN_FILENO);
		bufgl = NULL;
		if (eofflag)
			break;
	}
	close (infile);
	return (ret);
}

/**
 * main - Runs the shell
 * @ac: number of args
 * @av: commad line argument matrix
 * @environ: environment matrix
 *
 * Return: return value of last command
 */
int main(int ac, char *av[],char **environ)
{
	int ret = 0;
	char *pidptr;

	initsvars(ac - 1, av);
	pidptr = _getpid();
	setsvar("$", pidptr);
	free(pidptr);
	_getline(NULL, -2);
	setallenv(environ, NULL);
#ifdef DEBUGINIT
	printf("?:%s\n", getsvar("?"));
	printf("0:%s\n", getsvar("0"));
	setsvar(_strdup("simplevar"), _strdup("98"));
	printf("simplevar:%s\n", getsvar("simplevar"));
#endif
	if (ac > 1)
		ret = scriptmode(av);
	else
		ret = shintmode();
	exitcleanup(NULL);
	exit_hist();
	return (ret);
}
