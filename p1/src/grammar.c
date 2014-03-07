#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "grammar.h"

int grammar_init(struct grammar_t **g);
void grammar_free(struct grammar_t *g);

int grammar_connector_init(struct connector_t **connector);
void grammar_connector_free(struct connector_t *connector);
int grammar_connector_add(struct grammar_t *g, struct connector_t *c);

static int grammar_variable_init(const char *name, struct symbol_t **symbol);
void *grammar_variable_new(struct grammar_t *g, const char *name);
static void *grammar_variable_find(struct grammar_t *g, const char *name);

static int grammar_terminal_init(const char *name, struct symbol_t **symbol);
void *grammar_terminal_new(struct grammar_t *g, const char *name);
static void *grammar_terminal_find(struct grammar_t *g, const char *name);

static int grammar_symbol_init(const char *name, struct symbol_t **symbol, int type);
void grammar_symbol_free(struct symbol_t *symbol);


int grammar_init(struct grammar_t **g)
{
	struct grammar_t *grammar;

	grammar = (struct grammar_t *) malloc(sizeof(*grammar));

	if(!grammar)
	{
		perror("malloc");
		return -1;
	}

	/* Fill all with 0's and NULL */
	memset(grammar, 0, sizeof(*grammar));

	*g = grammar;

	return 0;
}

void grammar_free(struct grammar_t *g)
{
	int i;

	if(g)
	{
		if(g->connectors)
			for(i=0; i<g->connectors_n; i++)
			{
				grammar_connector_free(g->connectors[i]);
			}
			free(g->connectors);

		if(g->variables)
			for(i=0; i<g->variables_n; i++)
			{
				grammar_symbol_free(g->variables[i]);
			}
			free(g->variables);

		if(g->terminals)
			for(i=0; i<g->terminals_n; i++)
			{
				grammar_symbol_free(g->terminals[i]);
			}
			free(g->terminals);

		free(g);
	}
}



int grammar_connector_init(struct connector_t **connector)
{
	struct connector_t *c;

	c = malloc(sizeof(*c));
	if(c == NULL)
	{
		perror("malloc");
		return -1;
	}

	c->type = NODE_CON;
	c->from = NULL;
	c->to1 = NULL;
	c->to2 = NULL;

	*connector = c;
	return 0;
}

void grammar_connector_free(struct connector_t *connector)
{
	if(connector) free(connector);
}

int grammar_connector_add(struct grammar_t *g, struct connector_t *c)
{
	struct connector_t **connectors;
	long n;
	size_t size;

	n = g->connectors_n + 1;
	size = n * sizeof(struct connector_t *);
	
	connectors = realloc(g->connectors, size);
	
	if(!connectors)
	{
		perror("realloc");
		return -1;
	}

	connectors[n-1] = c;

	g->connectors = connectors;
	g->connectors_n = n;

	return 0;
}



static int grammar_variable_init(const char *name, struct symbol_t **symbol)
{
	return grammar_symbol_init(name, symbol, NODE_VAR);
}

void *grammar_variable_new(struct grammar_t *g, const char *name)
{
	struct symbol_t **variables;
	struct symbol_t *symbol;
	long n;
	size_t size;
	struct symbol_t *find;

	/* If it's duplicated */
	if(find = grammar_variable_find(g, name))
		return find;

	n = g->variables_n + 1;
	size = n * sizeof(struct symbol_t *);
	
	variables = realloc(g->variables, size);
	
	if(!variables)
	{
		perror("realloc");
		return NULL;
	}
	
	if(grammar_variable_init(name, &symbol) < 0)
		return NULL;

	variables[n-1] = symbol;

	g->variables = variables;
	g->variables_n = n;

	return symbol;
}

static void *grammar_variable_find(struct grammar_t *g, const char *name)
{
	int i;

	for(i=0; i<g->variables_n; i++)
	{
		if(strcmp(g->variables[i]->name, name) == 0)
		{
			return g->variables[i];
		}
	}

	return NULL;
}



static int grammar_terminal_init(const char *name, struct symbol_t **symbol)
{
	return grammar_symbol_init(name, symbol, NODE_TER);
}

void *grammar_terminal_new(struct grammar_t *g, const char *name)
{
	struct symbol_t **terminals;
	struct symbol_t *symbol;
	long n;
	size_t size;
	struct symbol_t *find;

	/* If it's duplicated */
	if(find = grammar_terminal_find(g, name))
		return find;

	n = g->terminals_n + 1;
	size = n * sizeof(struct symbol_t *);
	
	terminals = realloc(g->terminals, size);
	
	if(!terminals)
	{
		perror("realloc");
		return NULL;
	}
	
	if(grammar_terminal_init(name, &symbol) < 0)
		return NULL;

	terminals[n-1] = symbol;

	g->terminals = terminals;
	g->terminals_n = n;

	return symbol;
}

static void *grammar_terminal_find(struct grammar_t *g, const char *name)
{
	int i;

	for(i=0; i<g->terminals_n; i++)
	{
		if(strcmp(g->terminals[i]->name, name) == 0)
		{
			return g->terminals[i];
		}
	}

	return NULL;
}



static int grammar_symbol_init(const char *name, struct symbol_t **symbol, int type)
{
	struct symbol_t *s;
	char *n;

	s = (struct symbol_t *) malloc(sizeof(*s));
	if(s == NULL)
	{
		perror("malloc");
		return -1;
	}

	n = (char *) malloc(strlen(name) + 1);
	if(n == NULL)
	{
		free(s);
		perror("malloc");
		return -1;
	}

	strcpy(n, name);
	s->type = type;
	s->name = n;

	(*symbol) = s;
	return 0;
}

void grammar_symbol_free(struct symbol_t *symbol)
{
	if(symbol)
	{
		if(symbol->name)
		{
			free(symbol->name);
		}
		free(symbol);
	}
}



int main(int argc, char *argv[])
{
	struct grammar_t *g;
	struct connector_t *c;
	struct symbol_t *A, *B, *a, *b;

	if(grammar_init(&g) < 0)
	{
		return -1;
	}

	A=grammar_variable_new(g, "A");
	B=grammar_variable_new(g, "B");

	a=grammar_terminal_new(g, "a");
	b=grammar_terminal_new(g, "b");

	grammar_connector_init(&c);
	c->from = A;
	c->to1 = B;

	grammar_connector_add(g, c);

	grammar_free(g);

	return 0;
}
