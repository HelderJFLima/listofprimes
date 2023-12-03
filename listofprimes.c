/** by HelderJFLima **/

/*
* 
* This program creates a list of prime numbers up to a certain value entered
* by the user. It is possible to enter the maximum value via the command line
* or just call the program and wait for it to request it.
*
* For more information, look for the README file.
* 
*/


#include <ctype.h>		/* isblank, isdigit */
#include <direct.h>		/* _getcwd */
#include <errno.h>		/* For error messages */
#include <limits.h>		/* ULONG_MAX */
#include <math.h>		/* ceil, log, log10, sqrt */
#include <stdio.h>		/* fgets, perror, printf, fprintf, sprintf */
#include <stdlib.h>		/* calloc, free, strtoul */
#include <string.h>		/* strcat, strcpy, strlen */

#define BASE	   10					/* Base used in strtoul function */
#define EXTENSION  ".txt"				/* Extension of the file */
#define FILENAME   "list_of_primes_up_to_"
#define FIRSTPRIME 2					/* First prime number */
#define GETDIR     _getcwd				/* Function to get the directory name */
#define STRING1    "quantity="			/* String used in the file */
#define STRING2    ",last="				/* Ditto */

char* getfilename(char *firstpart, unsigned long number, char *extension);

unsigned long getfromarg(char *argument);

unsigned long getfromstdin(void);

int isprime(unsigned long number, unsigned long *primes);


/* listofprimes: create a list of prime numbers up to a given limit and save
				 the result to a file. */

int main(int argc, char *argv[])
{
	unsigned long maxNum;			/* Maximum number to be tested */
	unsigned long testNum;			/* Number to test if it is prime */
	unsigned long counter = 0;		/* To count the primes */
	unsigned long lsize;			/* Size of list of primes */
	unsigned long *plist;			/* List of primes */
	unsigned long *plp;				/* Auxiliar pointer */
	char *dname;					/* Directory name */
	char *fname;					/* File name */
	FILE *file;

	if(argc == 1)
	{
		maxNum = getfromstdin();						/* Get number */

		if(!maxNum)										/* Error */
		{
			printf("\nThe number must be an integer ");

			printf("from %lu to %lu\n\n", FIRSTPRIME, ULONG_MAX - 1);

			return 1;
		}
	}
	else if(argc == 2)
	{
		maxNum = getfromarg(*++argv);					/* Get number from */
														/* command-line    */

		if(!maxNum)
		{
			printf("\nThe number must be an integer ");

			printf("from %lu to %lu\n\n", FIRSTPRIME, ULONG_MAX - 1);

			return 2;
		}
	}
	else
	{
		printf("\nWrong number of arguments\n\n");

		return 3;
	}

									/* According to the Prime Number Theorem */
	lsize = (unsigned long)ceil( 1.3 * ( maxNum / log(maxNum) ) );

	plist = (unsigned long*)calloc( lsize, sizeof(unsigned long) );

	if(plist == NULL)
	{
		perror("\listofprimes->calloc");				/* Error message */

		return 4;
	}

	plist[counter++] = FIRSTPRIME;						/* First value */

	testNum = FIRSTPRIME + 1;

	while(testNum <= maxNum)
	{
		printf("\ntesting %lu", testNum);

		if( isprime(testNum, plist) )					/* Test the numbers */
		{
			plist[counter++] = testNum;

			printf("       *PRIME*");
		}
		
		testNum++;
	}

	fname = getfilename(FILENAME, maxNum, EXTENSION);	/* Get file name */

	file = fopen(fname, "w");							/* Create file */

	if(file == NULL)
	{
		perror("\listofprimes->fopen");

		free(plist);

		free(fname);

		return 5;
	}

	printf("\n\nWriting output file...\n");
													/* First line of the file */
	fprintf(file, "%s%lu%s%lu", STRING1, counter, STRING2, plist[counter - 1]);

	plp = plist;

	while(*plp)											/* Write the numbers */
		fprintf(file, "\n%lu", *plp++);					/* in the file       */

	fclose(file);										/* Close file */

	dname = GETDIR(NULL, _MAX_DIR);						/* Get the name of the */
														/* current directory   */

	if(dname != NULL)									/* Final message */
	{
		printf("\nThe list of primes was saved in:\n\n");

		printf("%s\\%s\n\n", dname, fname);
	}
	else
		printf("\nThe list of primes was saved in file %s\n\n", fname);

	free(plist);

	free(fname);

	free(dname);

	return 0;
}


/* getfilename: get a file name made of a string, a number and an extension;
				return a pointer to the string with the file name or NULL if
				an error ocurrs. */

char* getfilename(char *firstpart, unsigned long number, char *extension)
{
	int size;			/* Size of an array */
	char *filename;		/* String for the file name */
	char *nstring;		/* String for number */

	size = (int)ceil( log10(number) ) + 2;	/* +2 for the terminating null     */
											/* character and to avoid overflow */

	nstring = (char*)calloc( size, sizeof(char) );

	if(nstring == NULL)
	{
		perror("\getfilename->calloc");

		return NULL;
	}

	if( ( sprintf(nstring, "%lu", number) ) < 0 )	/* Write the number in    */
	{												/* the string and test if */
		perror("\getfilename->sprintf");			/* it worked              */

		free(nstring);

		return NULL;
	}

	size += ( strlen(firstpart) + strlen(extension) );

	filename = (char*)calloc( size, sizeof(char) );

	if (filename != NULL)
	{
		strcpy(filename, firstpart);

		strcat(filename, nstring);					/* Concatenate the strings */

		strcat(filename, extension);
	}
	else
		perror("\getfilename->calloc");

	free(nstring);

	return filename;
}


/* getfromarg: get a command-line argument and convert it to an unsigned long;
			   return 0 if an error occurs or the number is out of range.*/

unsigned long getfromarg(char *argument)
{
	unsigned long number;

	if( isdigit(*argument) )						/* Valid decimal number */
	{
		number = strtoul(argument, NULL, BASE);		/* Convert to unsigned long */

		if(number == ULONG_MAX)						/* Upper limit violation */
		{
			printf("\nThe number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\nFormat error\n");

		return 0;
	}

	return (number < FIRSTPRIME) ? 0 : number;
}


/* getfromstdin: get a string from stdin and convert it to an unsigned long;
				 return 0 if an error occurs or the number is out of range. */

unsigned long getfromstdin(void)
{
	unsigned long number;
	int length;										/* Length of string */
	char *string, *strp;

	length = 2 * (int)ceil( log10(ULONG_MAX) );		/* Twice the length of */
													/* a long int          */

	string = (char*)calloc( length, sizeof(char) );

	if(string == NULL)
	{
		perror("\ngetfromstdin->calloc");

		return 0;
	}

	printf("\nEnter the maximum number to test: ");

	if( !fgets(string, length, stdin) )				/* Get a number */
	{
		perror("\ngetfromstdin->fgets");

		free(string);

		return 0;
	}

	strp = string;

	while( isblank(*strp) )							/* Jump blanks */
		strp++;

	if( isdigit(*strp) )							/* Valid decimal number */
	{
		number = strtoul(string, NULL, BASE);		/* Convert to unsigned long */

		free(string);

		if(number == ULONG_MAX)						/* Upper limit violation */
		{
			printf("\nThe number is too large\n");

			return 0;
		}
	}
	else
	{
		printf("\nFormat error\n");

		free(string);

		return 0;
	}

	return (number < FIRSTPRIME) ? 0 : number;
}


/* isprime: check if a number is prime, based on a list of primes; return 1 if
			it is prime and 0 otherwise. */

int isprime(unsigned long number, unsigned long *primes)
{
	enum boolean {FALSE, TRUE};
	int answer = TRUE;
	unsigned long limit;

	limit = (unsigned long)ceil( sqrt(number) );		/* Test limit */

	if(number == 3 || number == 5)						/* This avoid some */
		return TRUE;									/* problems        */

	while(*primes <= limit)
	{
		if(number % *primes == 0)
		{
			answer = FALSE;

			break;
		}
		else
			primes++;									/* Next prime */
	}

	return answer;
}
