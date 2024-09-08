#include "shell.h"

/**
 * _strcmp - compares two strings
 * @s1: first string
 * @s2: second string
 *
 * Return: positive if s1 is greater than s2
 * negative if s1 is less than s2
 * 0 if they are the same
 */
int _strcmp(char *s1, char *s2)
{
	int i = 0;

	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
		{
			return (s1[i] - s2[i]);
		}
		i++;
	}
	return (s1[i] - s2[i]);
}

/**
 * _strlen - returns the length of a string
 * @str: the string to be measured
 *
 * Return: length of the string
 */
size_t _strlen(char *str)
{
	size_t length = 0;

	while (*str++)
		length++;
	return (length);
}
/**
 * _strcpy - copies a string
 * @dest: where to copy the string
 * @src: string source
 *
 * Return: copied string
 */
char *_strcpy(char *dest, char *src)
{
	char *ptr = dest;

	while (*src)
		*ptr++ = *src++;
	*ptr = *src;

	return (dest);
}
/**
 * _strdup - duplicates a string
 * @str: the string to duplicate
 *
 * Return: duplicated string
 */
char *_strdup(char *str)
{
	char *new;

	if (str == NULL)
		return (NULL);
	new = malloc(sizeof(char) * (_strlen(str) + 1));
	if (new == NULL)
		return (NULL);
	_strcpy(new, str);
	return (new);
}
/**
 * _strcat - concatenates strings
 * @dest: second string
 * @src: first string
 *
 * Return: concatenated string
 */
char *_strcat(char *dest, char *src)
{
	char *ptrs = dest;

	while (*ptrs)
		ptrs++;
	while (*src)
		*ptrs++ = *src++;
	*ptrs = 0;
	return (dest);
}
