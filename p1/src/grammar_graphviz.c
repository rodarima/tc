#include "grammar_graphviz.h"

#include <stdio.h>

#include "grammar.h"
#include "list.h"

void grammar_graphviz_connector_simple_print(struct connector_t *c)
{
	struct symbol_t *s;

	if(NODE_IS_TYPE(c->from, NODE_CON))
	{
		printf("\"%p\" -> \"%s\";\n", c, c->sym->name);
	}
	else
	{
		s = (struct symbol_t *) c->from;
		printf("\"%s\" -> \"%p\";\n", s->name, c);
		printf("\"%p\" -> \"%s\";\n", c, c->sym->name);
	}
}

void grammar_graphviz_connector_complex_print(struct connector_t *c)
{
	grammar_graphviz_connector_simple_print(c);
	printf("\"%p\" -> \"%p\";\n", c, c->con);
}

void grammar_graphviz_connector_print(struct connector_t *c)
{

	if(c->con == NULL)
	{
		grammar_graphviz_connector_simple_print(c);
	}
	else
	{
		grammar_graphviz_connector_complex_print(c);
	}
}

void grammar_graphviz_print(struct grammar_t *g)
{
	list_map(&(g->connectors), (void (*)(void *)) grammar_graphviz_connector_print);
}

