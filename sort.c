/*!
 * @file sort.c
 * @brief program sorts input text from the beginning of strings and from ending
 * @author Sergei Kachkov
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*!
 * getting data to buffer from the text file
 * @param filename path (absolute or relative) to text file
 * @param buf pointer to pointer to data buffer
 * @warning buf must be free after using!
 * @return size of buffer in bytes
 */
size_t get_data (const char *filename, char **buf)
{
	FILE *f = fopen (filename, "rb");
	assert (f);
	
	fseek (f, 0, SEEK_END);
	size_t buf_size = ftell (f);
	
	*buf = malloc (buf_size);
	assert (*buf);
	fseek (f, 0, SEEK_SET);
	fread (*buf, buf_size, 1, f);
	
	fclose (f);
	return buf_size;
}

/*!
 * calculates number of strings in buffer
 * @param buf pointer to pointer to data buffer
 * @param size size  of buffer in bytes
 * @return number of strings
 */
size_t strings_number (const char *buf, size_t size)
{
	size_t ans = 0;
	const char *cur_symb = buf;
	while (cur_symb - buf < size)
		if (*cur_symb++ == '\n')
			ans++;
	return ans + 1;
}

/*!
 * extract all strings in buf; adds zero to end and returns number of successfully extracted strings
 * @param buf pointer to pointer to data buffer
 * @param strings pointer to array that will contain pointers to strings
 * @param size size of buffer in bytes
 * @param num number of strings
 * @return number of succesfully extracted strings
 */
size_t extract_strings (char *buf, char **strings, size_t size, size_t num)
{
	char *cur_symb = buf;
	for (size_t i = 0; i < num; i++)
	{
		strings[i] = cur_symb;
		while ((cur_symb - buf) < size && *cur_symb != '\n')
			cur_symb++;
		if (*cur_symb == '\n')
			{
				*cur_symb = '\0';
				cur_symb++;
			}
		else
			return i+1;
	}
	return size;
}

/*!
 * C function isalpha (char) doesn't support locale; that's analog for it
 * @note function doesn't support utf-8; input data must be in one-byte encoding
 * @note use std::isalpha (std::locale) in C++
 * @param c character
 * @return if character alphabetical, returns 1; else 0
 */
int rus_isalpha (char c)
{
	if (('a'<=c && c<='z') ||
		('A'<=c && c<='Z') ||
		('à'<=c && c<='ÿ') ||
		('À'<=c && c<='ß'))
		return 1;
	return 0;
}

/*!
 * compare two strings from the begin
 * @param *arg1 pointer to pointer to first string
 * @param *arg2 pointer to pointer to second string
 * @return 1, if arg1 more than arg2; 0, if arg1 and arg2 are equal; -1, if arg1 less than arg2
 */
int cmp_from_begin (const void *arg1, const void *arg2)
{
	char *a1 = *(char **)arg1;
	char *a2 = *(char **)arg2;
	
	while (*a1  || *a2 )
	{
		while (*a1 && !rus_isalpha(*a1))
			a1++;
		while (*a2 && !rus_isalpha(*a2))
			a2++;
		int cmp = *a1 - *a2;
		if (cmp)
			return cmp;
		if (*a1)
			a1++;
		if (*a2)
			a2++;
	}
	return 0;
}

/*!
 * compare two strings from the end
 * @param *arg1 pointer to pointer to first string
 * @param *arg2 pointer to pointer to second string
 * @return 1, if arg1 more than arg2; 0, if arg1 and arg2 are equal; -1, if arg1 less than arg2
 */
int cmp_from_end (const void *arg1, const void *arg2)
{
	char *a1 = *(char **)arg1 + strlen (*(char **)arg1) - 1;
	char *a2 = *(char **)arg2 + strlen (*(char **)arg2) - 1;
	
	while (a1 >= *(char **)arg1 || a2 >= *(char **)arg2)
	{
		while (a1 > *(char **)arg1 && !rus_isalpha(*a1))
			a1--;
		while (a2 > *(char **)arg2 && !rus_isalpha(*a2))
			a2--;
		int cmp = *a1 - *a2;
		if (cmp)
			return cmp;
		if (a1 >= *(char **)arg1)
			a1--;
		if (a2 >= *(char **)arg2)
			a2--;
	}
	return 0;
}

int main (int argc, char *argv[])
{
	if (argc < 4)
	{
		printf ("Correct format: sort  <filename> <output1> <output2>\n");
		return 0;
	}
	// getting information from file
	char *data = NULL;
	size_t data_size = get_data (argv[1], &data);
	size_t strings_num = strings_number (data, data_size);
	char **strings = calloc (strings_num, sizeof (char *));
	extract_strings (data, strings, data_size, strings_num);
	
	char **str_sort = calloc (strings_num, sizeof (char *));
	
	// sort from beginning
	memcpy (str_sort, strings, sizeof (char *) * strings_num);
	qsort (str_sort, strings_num, sizeof (char *), cmp_from_begin);
	
	FILE *f = fopen (argv[2], "w");
	assert (f);
	for (int i = 0; i<strings_num; i++)
		fprintf (f, "%s\n", str_sort[i]);
	fclose (f);
	
	// sort from end
	memcpy (str_sort, strings, sizeof (char *) * strings_num);
	qsort (str_sort, strings_num, sizeof (char *), cmp_from_end);
	
	f = fopen (argv[3], "w");
	assert (f);
	for (int i = 0; i<strings_num; i++)
		fprintf (f, "%s\n", str_sort[i]);
	fclose (f);
	
	free (str_sort);
	
	free (strings);
	free (data);
	return 0;
}
