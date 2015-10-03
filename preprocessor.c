/**
 * @file preprocessor.c
 * program deletes from text empty strings, numbers of chapters, empty chapters and footnotes
 * @brief preprocessing text for sort
 * @author Sergei Kachkov
 */
#include <stdio.h>
#include <string.h>

#define MAX_STRING_LENGTH 256

int IsSignificant (const char *s)
{
	for (int i = 0; i < strlen (s); i++)
		if (!strchr("IVХXL. \n", s[i]))
			return 0;
	return 1;
}

void DeleteFootnotes (char *s)
{
	for (int i = 0; i < strlen(s); i++)
		if (s[i] == '{')
		{
			int j = i + 1;
			while (s[j] && s[j] != '}')
				j++;
			if (s[j])
				memmove ((void *)(s + i - 1), (void *)(s + j +1), strlen (s + j + 1) + 1);
			else
				s[i] = '\0';
		}
}

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		printf ("Correct format: preprocessor <filename>\n");
		return 0;
	}
	
	FILE *f1 = fopen (argv[1], "r");
	
	if (!f1)
	{
		printf ("Can not find file: %s\n", argv[1]);
		return 0;
	}
	
	char filename [MAX_STRING_LENGTH];
	strcpy (filename, "ans_");
	strcat (filename, argv[1]);
	
	FILE *f2 = fopen (filename, "w");
	
	char str [MAX_STRING_LENGTH];
	
	while (!feof (f1))
	{
		fgets (str, MAX_STRING_LENGTH, f1);
		
		if (IsSignificant (str))
			continue;
		if (strstr(str, "ГЛАВА") == str)
			continue;
		DeleteFootnotes (str);
		
		fputs (str, f2);	
	}
	
	fclose (f2);
	fclose (f1);
	return 0;
}
