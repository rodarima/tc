#include "parse_std.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "constants.h"
#include "grammar.h"
#include "grammar_connect.h"
#include "dbg.h"

#define SYM_EQUAL	'='
#define SYM_SEPAR	','
#define SYM_QUOTE	'"'
#define SYM_SPECIAL	'\\'
#define SYM_NULL	'\0'

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
	char c;
	int state;

	state = 0;

	for(i=0; i<strlen(src)+1; i++)
	{
		c = src[i];

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
	printf("Found '\\0' at %d (not expected)\n", i);
	return 0;
}
/*
int init_generator(const char *name, struct node_generator_t **gen)
{
	struct node_generator_t *g;
	char *n;

	g = (struct node_generator_t *) malloc(sizeof(*gen));
	if(g == NULL)
	{
		return -1;
	}

	n = (char *) malloc(strlen(name));
	if(n == NULL)
	{
		free(g);
		return -2;
	}

	strcpy(g->name, name);
	g->type = NODE_GEN;

	(*gen) = g;
	return 0;
}

int grammar_add_generator(const char *name, struct grammar_t *g)
{
	struct node_generator_t *generator_new;
	int generators_n;

	if(init_generator(name, &generator_new) < 0)
		return -1;

	generators_n= g->generators_n;
	printf("Comprobar que no sea el mismo generador :)\n");
	g->generators[generators_n] = generator_new;
	g->generators_n++;
}
*/
/* Add the rule to the grammar. The rule must be in the right format:
 *
 * 	A=B,"literal",CC
 * 	B="aa"
 * 	CC="cc"
 * 	CC=B
 */
int parse_std_add_rule(const char *rule, struct grammar_t *grammar)
{
	int i, t=0;
	char c;
	int state;
	char tmp[MAX_LINE_SIZE];
	/* TODO: Change this pointers to static memory, adding at the end */
	struct connector_t *conn, *conn2;
	struct symbol_t *sym;


	#define BAD_LEFT	"Syntax error while evaluating the left side of the rule. \n"
	#define BAD_RIGTH	"Syntax error while evaluating the right side of the rule. \n"
	#define FOUND_CHAR_AT	"Found bad char '%c' (0x%x) at position %d"

	/* Get the left part of the rule */
	state = 0;

	#warning "Reduce this function in subfunctions"

	for(i=0; i<strlen(rule)+1; i++)
	{
		c = rule[i];

		switch(state) {
		case 0: /* At start */
			if(isalpha(c))
			{
				/* Add the first alpha to the name */
				tmp[t++]=c;

				state = 1;
			}
			else
			{
				printf(BAD_LEFT FOUND_CHAR_AT " (but expected alpha)\n", 
					c, c, i);
				return -10;
			}
			break;

		case 1: /* At firsts chars */
			if(isalpha(c))
			{
				state = 1;

				/* Add the rest of chars to the name */
				tmp[t++] = c;
			}
			else if(c == SYM_EQUAL)
			{
				/* End variable creation */
				tmp[t++]='\0';
				//printf("New variable (rule) called'%s'\n", tmp);

				/* Terminate variable creation */
				return_if(grammar_symbol_new(grammar, &sym, tmp, NODE_VAR), -1);
				return_if(grammar_connector_new(grammar, &conn), -1);
				return_if(grammar_connect_from_symbol(conn, sym), -1)
				if(grammar->start == NULL)
				{
					grammar->start = sym;
				}

				state = 2;
				t = 0;
			}
			else
			{
				printf(BAD_LEFT FOUND_CHAR_AT " (but expected alpha or '%c')\n",
					c, c, i, SYM_EQUAL);
				return -11;
			}
			break;

		case 2: /* At right side */
			if(c == SYM_QUOTE)
			{
				state = 3;
			}
			else if(isalpha(c))
			{
				/* Add start symbol of the variable */
				tmp[t++] = c;

				state = 6;
			}
			else
			{
				printf(BAD_RIGTH FOUND_CHAR_AT " (but expected alpha or '%c')\n",
					c, c, i, SYM_QUOTE);
				return -12;
			}
			break;

		case 3: /* At quoted string */
			if(c == SYM_QUOTE)
			{
				/* End terminal here */
				tmp[t++] = '\0';
				//printf("New terminal called '%s'\n", tmp);

				/* Add new terminal */
				return_if(grammar_symbol_new(grammar, &sym, tmp, NODE_TER), -1);
				return_if(grammar_connect_to_symbol(conn, sym), -1)

				state = 4;
				t = 0;
			}
			else if(c == SYM_SPECIAL)
			{
				state = 5;
			}
			else if(c == SYM_NULL)
			{
				printf(BAD_RIGTH FOUND_CHAR_AT " (but expected '%c', '%c' or the end)\n",
					c, c, i, SYM_QUOTE, SYM_SPECIAL);
				return -13;
			}
			else
			{
				/* Add new terminal char to the name */
				tmp[t++] = c;
			}
			break;

		case 4: /* Ended quoted string */
			if(c == SYM_SEPAR)
			{
				/* Concatenate more symbols or terminals */

				return_if(grammar_connector_new(grammar, &conn2), -1);
				grammar_connect_to_connector(conn, conn2);
				conn = conn2;
				conn2 = NULL;

				state = 2;
			}
			else if(c == SYM_NULL)
			{

				/* ---------- READY --------- */
				//printf("READY FOR ROCK!\n");

				return 0;
			}
			else
			{
				printf(BAD_RIGTH FOUND_CHAR_AT " (but expected '%c' or the end)\n",
					c, c, i, SYM_SEPAR);
				return -14;
			}
			break;

		case 5: /* At special char in terminal */
			if(c == SYM_SPECIAL || c == SYM_QUOTE)
			{
				/* Add special char to the terminal */
				tmp[t++] = c;

				state = 3;
			}
			else
			{
				printf(BAD_RIGTH FOUND_CHAR_AT " (but expected '%c' or '%c')\n",
					c, c, i, SYM_SEPAR, SYM_QUOTE);
				return -15;
			}
			break;

		case 6: /* After a alpha in the right side */
			if(isalpha(c))
			{
				/* Add the char to the generator name */
				tmp[t++] = c;
			}
			else if(c == SYM_SEPAR)
			{
				/* End variable creation */
				tmp[t++] = '\0';
				//printf("New variable called '%s'\n", tmp);

				/* Add new variable */
				return_if(grammar_symbol_new(grammar, &sym, tmp, NODE_VAR), -1);
				return_if(grammar_connect_to_symbol(conn, sym), -1);

				/* Concatenate more symbols or terminals */
				return_if(grammar_connector_new(grammar, &conn2), -1);
				grammar_connect_to_connector(conn, conn2);
				conn = conn2;
				conn2 = NULL;

				state = 2;
				t = 0;
			}
			else if(c == SYM_NULL)
			{
				tmp[t++] = '\0';
				//printf("Last variable called '%s'\n", tmp);

				/* Add new variable */
				return_if(grammar_symbol_new(grammar, &sym, tmp, NODE_VAR), -1);
				return_if(grammar_connect_to_symbol(conn, sym), -1);

				/* ---------- READY --------- */
				//printf("READY FOR ROCK!\n");

				return 0;
			}
			else
			{
				printf(BAD_RIGTH FOUND_CHAR_AT " (but expected alpha, '%c' or the end)\n",
					c, c, i, SYM_SEPAR);
				return -16;
			}
			break;


		default: /* How we arrived here? */
			printf("FATAL ERROR, bug detected\n");
			return -20;
		}
	}

	/* Or here? */
	printf("Found '\\0' at %d (not expected)\n", i);
	return -21;
}
