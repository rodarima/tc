#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "grammar.h"
#include "list.h"
#include "dbg.h"

int grammar_cmp_ptr(void *a, void *b);
int grammar_cmp_str_symbol(const void *a, const void *b);
static int grammar_symbol_init(struct symbol_t **symbol, const char *name, int type);
int grammar_connector_add(struct grammar_t *g, struct connector_t *c);


int grammar_init(struct grammar_t **g)
{
	*g = (struct grammar_t *) calloc(1, sizeof(struct grammar_t));

	return_if(!*g, -1);

	return 0;
}

void grammar_free(struct grammar_t *g)
{
	list_clear_func(&(g->connectors), (void (*)(void *)) grammar_connector_free);
	list_clear_func(&(g->variables), (void (*)(void *)) grammar_symbol_free);
	list_clear_func(&(g->terminals), (void (*)(void *)) grammar_symbol_free);
	free(g);
}

void grammar_print(struct grammar_t *g)
{
#ifndef NDEBUG
	debug("Printing grammar");
	debug("CONNECTORS:");
#endif
	list_map(&(g->connectors), (void (*)(void *)) grammar_connector_print);
#ifndef NDEBUG
	debug("VARIABLES:");
#endif
	list_map(&(g->variables), (void (*)(void *)) grammar_symbol_print);
#ifndef NDEBUG
	debug("TERMINALS:");
#endif
	list_map(&(g->terminals), (void (*)(void *)) grammar_symbol_print);
}

int grammar_connector_new(struct grammar_t *g, struct connector_t **connector)
{
	*connector = calloc(1, sizeof(struct connector_t));
#ifndef NDEBUG
	debug("Adding new connector [%p]", *connector);
#endif
	return_if(!*connector, -1);

	(*connector)->type = NODE_CON;

	grammar_connector_add(g, *connector);

	return 0;
}

/* FIXME: Only free a connector after disconnection */
void grammar_connector_free(struct connector_t *connector)
{
	free(connector);
}

int grammar_connector_add(struct grammar_t *g, struct connector_t *c)
{
	return_if(list_add(&(g->connectors), (void *) c), -1);
	return 0;
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

int grammar_cmp_ptr(void *a, void *b)
{
	return a==b;
}

int grammar_cmp_str_symbol(const void *a, const void *b)
{
	struct symbol_t *s;
	s = (struct symbol_t *) b;

	return strcmp((char *) a, s->name);
}


/* FIXME */
void grammar_node_print(void *node)
{
	int type;

	type = *((char*) node) & 0x03;
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

int grammar_symbol_new(struct grammar_t *g, struct symbol_t **symbol, const char *name, char type)
{
	struct symbol_t *s;
	struct list_t *list;

#ifndef NDEBUG
	debug("Adding new symbol %s", name);
#endif

	if(type == NODE_VAR)
	{
		list = &(g->variables);
	}
	else
	{
		list = &(g->terminals);
	}
	
	s = list_find(list, name, (int (*)(const void *, const void *)) grammar_cmp_str_symbol);
	if(s)
	{
		*symbol = s;
		return 0;
	}

	return_if(grammar_symbol_init(symbol, name, type), -1);

	if(list_add(list, *symbol) != 0)
	{
		grammar_symbol_free(*symbol);
		return -1;
	}

	return 0;
}

static int grammar_symbol_init(struct symbol_t **symbol, const char *name, int type)
{
	char *n;

	n = malloc(strlen(name) + 1);
	return_if(n == NULL, -1);
	*symbol = calloc(1, sizeof(struct symbol_t));
	if(*symbol == NULL)
	{
		free(n);
		return -1;
	}

	(*symbol)->type = type;
	(*symbol)->name = strcpy(n, name);
	return 0;
}

void grammar_symbol_free(struct symbol_t *symbol)
{
	list_clear(&(symbol->to));
	list_clear(&(symbol->from));
	free(symbol->name);
	free(symbol);
}

void grammar_symbol_print(struct symbol_t *s)
{
	if(NODE_IS_TYPE(s, NODE_VAR))
	{
		printf("Variable [%p]: %s\nTO:\n", s, s->name);
		list_map(&(s->to), (void (*)(void *)) grammar_connector_print);
		printf("FROM:\n");
		list_map(&(s->from), (void (*)(void *)) grammar_connector_print);
	}
	else if(NODE_IS_TYPE(s, NODE_TER))
	{
		printf("Terminal [%p]: %s\nTO:\n", s, s->name);
		list_map(&(s->to), (void (*)(void *)) grammar_connector_print);
		printf("FROM:\n");
		list_map(&(s->from), (void (*)(void *)) grammar_connector_print);
	}
	else
	{
		printf("ERROR, unknown type [%p]\n", s);
	}
}

/* Asegura que todas las variables sean generadores, es decir, que se puedan
 * transformar en un símbolo terminal */
//int grammar_clean_no_generators(struct grammar_t *g)
//{
//	/* Recorro todos los terminales */
//	void **list = NULL;
//	size_t list_n = 0;
//
//	long i, j;
//	struct connector_t *con;
//	struct symbol_t *sym;
//	void *node;
//
//	printf("Cleanning no-generators\n");
//
//	for(i=0; i < g->connectors_n; i++)
//	{
//		con = g->connectors[i];
//		
//		for(j=0; j < g->terminals_n; j++)
//		{
//			sym = g->terminals[j];
//
//			/* Si la conexión termina en un terminal, y el emisor
//			 * no está marcado, añadir el emisor */
//			if((con->sym == sym) && !NODE_IS_MARKED(con->from, MARK_PASS))
//			{
//				/* TODO: Add sym->from to the list */
//				#warning "TODO: Create a GOOD list"
//				
//				list = realloc(list, ++list_n * sizeof(*list));
//				if(!list)
//				{
//					perror("realloc");
//					free(list);
//					return -1;
//				}
//				printf("From ");
//				grammar_node_print(sym);
//				printf(" adding ");
//				grammar_node_print(con->from);
//				printf("\n");
//
//				/* Marcar el emisor */
//				NODE_MARK(con->from, MARK_PASS);
//				/* Añadir el emisor */
//				list[list_n-1] = con->from;
//
//				/* Salimos, ya que una conexión sólo puede
//				 * desembocar en un sólo símbolo */
//				break;
//			}
//		}
//	}
//	/* Mientras sigan existiendo nodos que explorar */
//	while(list_n > 0)
//	{
//		/* Elijo el último para avanzar en profundidad */
//		node = list[list_n-1];
//
//		/* Si es un conector ya tendrá la referencia con el padre */
//		if(NODE_IS_TYPE(node, NODE_CON))
//		{
//			con = node;
//			/* Marcar al padre */
//			NODE_MARK(con->from, MARK_PASS);
//			/* Añadir al padre */
//			list[list_n-1] = con->from;
//
//			/* Fin */
//			continue;
//		}
//		if(!NODE_IS_TYPE(node, NODE_VAR))
//		{
//			printf("FATAL: ESTO NUNCA DEBE SUCEDER\n");
//		}
//		
//		/* Si no, es una variable y busco enlaces que terminen en el */
//		
//		sym = node;
//		
//		/* Extraigo el nodo de la lista */
//		list_n--;
//
//		for(i=0; i < g->connectors_n; i++)
//		{
//			con = g->connectors[i];
//			
//			/* Si encontramos una conexión con el nodo que no esté marcada */
//			if((con->sym == sym) && !NODE_IS_MARKED(con->from, MARK_PASS))
//			{
//				/* Lo añadimos a la lista, sobreescribiendo el actual */
//				list = realloc(list, ++list_n * sizeof(*list));
//				if(!list)
//				{
//					perror("realloc");
//					free(list);
//					return -1;
//				}
//				printf("From ");
//				grammar_node_print(sym);
//				printf(" adding ");
//				grammar_node_print(con->from);
//				printf("\n");
//
//				/* Marcar el emisor */
//				NODE_MARK(con->from, MARK_PASS);
//				/* Añadir el emisor */
//				list[list_n-1] = con->from;
//				
//			}
//		}
//
//	}
//
//	if(list) free(list);
//
//	printf("Marked variables are:\n");
//	for(i=0; i<g->variables_n; i++)
//	{
//		sym = g->variables[i];
//		if(NODE_IS_MARKED(sym, MARK_PASS))
//		{
//			grammar_symbol_print(sym);
//		}
//	}
//
//	printf("Unmarked variables are:\n");
//	for(i=0; i<g->variables_n; i++)
//	{
//		sym = g->variables[i];
//		if(!NODE_IS_MARKED(sym, MARK_PASS))
//		{
//			grammar_symbol_print(sym);
//		}
//	}
//
//
//	return 0;
//
//}

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
