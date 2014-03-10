#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "grammar.h"

int grammar_init(struct grammar_t **g);
void grammar_free(struct grammar_t *g);
void grammar_print(struct grammar_t *g);

int grammar_connector_init(struct connector_t **connector);
void grammar_connector_free(struct connector_t *connector);
int grammar_connector_add(struct grammar_t *g, struct connector_t *c);
void grammar_connector_print(struct connector_t *c);

static int grammar_variable_init(const char *name, struct symbol_t **symbol);
void *grammar_variable_new(struct grammar_t *g, const char *name);
static void *grammar_variable_find(struct grammar_t *g, const char *name);

static int grammar_terminal_init(const char *name, struct symbol_t **symbol);
void *grammar_terminal_new(struct grammar_t *g, const char *name);
static void *grammar_terminal_find(struct grammar_t *g, const char *name);

static int grammar_symbol_init(const char *name, struct symbol_t **symbol, int type);
void grammar_symbol_free(struct symbol_t *symbol);
void grammar_symbol_print(struct symbol_t *s);

void grammar_node_print(void *node);


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
		{
			for(i=0; i<g->connectors_n; i++)
			{
				grammar_connector_free(g->connectors[i]);
			}
			free(g->connectors);
		}
		if(g->variables)
		{
			for(i=0; i<g->variables_n; i++)
			{
				grammar_symbol_free(g->variables[i]);
			}
			free(g->variables);
		}
		if(g->terminals)
		{
			for(i=0; i<g->terminals_n; i++)
			{
				grammar_symbol_free(g->terminals[i]);
			}
			free(g->terminals);
		}
		free(g);
	}
}

void grammar_print(struct grammar_t *g)
{
	int i;

	if(g->connectors)
	{
		for(i=0; i<g->connectors_n; i++)
		{
			grammar_connector_print(g->connectors[i]);
		}
	}
	if(g->variables)
	{
		for(i=0; i<g->variables_n; i++)
		{
			grammar_symbol_print(g->variables[i]);

		}
	}
	if(g->terminals)
	{
		for(i=0; i<g->terminals_n; i++)
		{
			grammar_symbol_print(g->terminals[i]);
		}
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
	c->sym = NULL;
	c->con = NULL;

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

void grammar_node_print(void *node)
{
	if(!node) printf("NULL");
	int type = *((char*) node) & 0x03;
	switch(type)
	{
		case NODE_VAR:
			printf("VAR(%s)", ((struct symbol_t *)node)->name);
			break;
		case NODE_TER:
			printf("TER(%s)", ((struct symbol_t *)node)->name);
			break;
		case NODE_CON:
			printf("CON(%p)", node);
			break;
		default:
			printf("ERROR, type not identified");
			break;
	}
}
char grammar_node_type(void *node)
{
	return *((char*) node);
}

void grammar_connector_print(struct connector_t *c)
{
	if(c->con == NULL)
	{
		printf("Simple connector  [%p]: ",c);
		grammar_node_print(c->from);
		printf(" -> ");
		grammar_node_print(c->sym);
		printf("\n");
	}
	else
	{
		printf("Complex connector [%p]: ",c);
		grammar_node_print(c->from);
		printf(" -> ");
		grammar_node_print(c->sym);
		printf(" +-> ");
		grammar_node_print(c->con);
		printf("\n");
	}
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

void grammar_symbol_print(struct symbol_t *s)
{
	if(NODE_IS_TYPE(s, NODE_VAR))
	{
		printf("Variable [%p]: %s\n", s, s->name);
	}
	else if(NODE_IS_TYPE(s, NODE_TER))
	{
		printf("Terminal [%p]: %s\n", s, s->name);
	}
	else
	{
		printf("ERROR, unknown type [%p]\n", s);
	}
}

/* Asegura que todas las variables sean generadores, es decir, que se puedan
 * transformar en un símbolo terminal */
int grammar_clean_no_generators(struct grammar_t *g)
{
	/* Recorro todos los terminales */
	void **list = NULL;
	size_t list_n = 0;

	long i, j;
	struct connector_t *con;
	struct symbol_t *sym;
	void *node;

	printf("Cleanning no-generators\n");

	for(i=0; i < g->connectors_n; i++)
	{
		con = g->connectors[i];
		
		for(j=0; j < g->terminals_n; j++)
		{
			sym = g->terminals[j];

			/* Si la conexión termina en un terminal, y el emisor
			 * no está marcado, añadir el emisor */
			if((con->sym == sym) && !NODE_IS_MARKED(con->from, MARK_PASS))
			{
				/* TODO: Add sym->from to the list */
				#warning "TODO: Create a GOOD list"
				
				list = realloc(list, ++list_n * sizeof(*list));
				if(!list)
				{
					perror("realloc");
					free(list);
					return -1;
				}
				printf("From ");
				grammar_node_print(sym);
				printf(" adding ");
				grammar_node_print(con->from);
				printf("\n");

				/* Marcar el emisor */
				NODE_MARK(con->from, MARK_PASS);
				/* Añadir el emisor */
				list[list_n-1] = con->from;

				/* Salimos, ya que una conexión sólo puede
				 * desembocar en un sólo símbolo */
				break;
			}
		}
	}
	/* Mientras sigan existiendo nodos que explorar */
	while(list_n > 0)
	{
		/* Elijo el último para avanzar en profundidad */
		node = list[list_n-1];

		/* Si es un conector ya tendrá la referencia con el padre */
		if(NODE_IS_TYPE(node, NODE_CON))
		{
			con = node;
			/* Marcar al padre */
			NODE_MARK(con->from, MARK_PASS);
			/* Añadir al padre */
			list[list_n-1] = con->from;

			/* Fin */
			continue;
		}
		if(!NODE_IS_TYPE(node, NODE_VAR))
		{
			printf("FATAL: ESTO NUNCA DEBE SUCEDER\n");
		}
		
		/* Si no, es una variable y busco enlaces que terminen en el */
		
		sym = node;
		
		/* Extraigo el nodo de la lista */
		list_n--;

		for(i=0; i < g->connectors_n; i++)
		{
			con = g->connectors[i];
			
			/* Si encontramos una conexión con el nodo que no esté marcada */
			if((con->sym == sym) && !NODE_IS_MARKED(con->from, MARK_PASS))
			{
				/* Lo añadimos a la lista, sobreescribiendo el actual */
				list = realloc(list, ++list_n * sizeof(*list));
				if(!list)
				{
					perror("realloc");
					free(list);
					return -1;
				}
				printf("From ");
				grammar_node_print(sym);
				printf(" adding ");
				grammar_node_print(con->from);
				printf("\n");

				/* Marcar el emisor */
				NODE_MARK(con->from, MARK_PASS);
				/* Añadir el emisor */
				list[list_n-1] = con->from;
				
			}
		}

	}

	if(list) free(list);

	printf("Marked variables are:\n");
	for(i=0; i<g->variables_n; i++)
	{
		sym = g->variables[i];
		if(NODE_IS_MARKED(sym, MARK_PASS))
		{
			grammar_symbol_print(sym);
		}
	}
	printf("Unmarked variables are:\n");
	for(i=0; i<g->variables_n; i++)
	{
		sym = g->variables[i];
		if(!NODE_IS_MARKED(sym, MARK_PASS))
		{
			grammar_symbol_print(sym);
		}
	}


	return 0;

}

/*
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
	c->sym = B;

	grammar_connector_add(g, c);

	grammar_free(g);

	return 0;
}
*/
