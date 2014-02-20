#include "parse_std.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SYM_EQUAL	'='
#define SYM_SEPAR	','
#define SYM_QUOTE	'"'
#define SYM_SPECIAL	'\\'

char *equal_position(const char *str)
{
	char *first;
	char *last;

	/* First occurrence */
	first = strchr(str, SYM_EQUAL);

	/* Last occurrence */
	last = strrchr(str, SYM_EQUAL);

	/* Not found or more than one equal? */
	if((first == NULL) || (first != last))
		return NULL;

	return first;
}

int is_std_rule(const char *str)
{
	/* Only one = */
	if(!strchr(str, SYM_EQUAL)) return 0;

	return 1;
}

/* Returns the left part of the rule */
void parse_std_left(const char *src, char *dst)
{
	while(*src != SYM_EQUAL)
	{
		*dst++=*src++;
	}
	*dst='\0';
}

/* Returns the right part of the rule */
void parse_std_right(const char *src, char *dst)
{
	while(*src != SYM_EQUAL) src++;
	
	src++;
	
	while(*src != '\0')
	{
		*dst++ = *src++;
	}
	*dst='\0';
}

/* Is src a left part of a rule? */
int is_std_rule_left(const char *src)
{
	while(*src != '\0')
	{
		if(!isalpha(*src++)) return 0;
	}
	return 1;
}

/* Is src a right part of a rule? */
int is_std_rule_right(const char *src)
{
	int i;
	char c,d;
	int state;

	state = 0;

	for(i=0; i<strlen(src)+1; i++)
	{
		c = src[i];
		printf("State %d and %c\n", state, c);

		switch(state) {
		case 0: /* At start */
			if(isalpha(c)) 
				state = 2;
			else if(c==SYM_QUOTE)
				state = 1;
			else
			{
				printf("Found '%c' at %d (expected alpha, '%c' or '\\0')\n", c, i,
					SYM_QUOTE);
				return 0;
			}

			break;
		case 1:	/* We are quoted */
			if(c==SYM_QUOTE)
				state = 3;
			else if(c==SYM_SPECIAL)
				state = 4;
			break;
		case 2: /* We are in a symbol */
			if(c==SYM_SEPAR)
				state = 0;
			else if(c=='\0')
				return 1;
			else if(!isalpha(c))
			{
				printf("Found '%c' at %d (expected alpha,'%c' or '\\0')\n", c, i,
					SYM_SEPAR);
				return 0;
			}
			break;
		case 3: /* We are at the end of a quote */
			if(c==SYM_SEPAR)
				state = 0;
			else if(c=='\0')
				return 1;
			else
			{
				printf("Found '%c' at %d (expected '%c' or '\\0')\n", c, i,
					SYM_SEPAR);
				return 0;
			}
			break;
		case 4:
			/* We are in a backslash */
			if(c==SYM_QUOTE || c==SYM_SPECIAL)
				state = 1;
			else
			{
				printf("Found '%c' at %d (expected '%c' or '%c')\n", c, i,
					SYM_QUOTE, SYM_SPECIAL);
				return 0;
			}
			break;
		default: /* How we arrived here? */
			return 0;
		}
	}

	/* Or here? */
	printf("End not expected\n");
	return 0;
}
