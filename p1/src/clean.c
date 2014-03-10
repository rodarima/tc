#include "grammar.h"


/* Asegura que todas las variables sean generadores, es decir, que se puedan
 * transformar en un símbolo terminal */
int grammar_clean_no_generators(struct grammar_t *g)
{
	/* Recorro todos los terminales */
	void **list;
	size_t list_n = 0;

	long i, j;
	struct connector_t *con;
	struct symbol_t *sym;

	for(i=0; i < g->connectors_n; i++)
	{
		con = g->connectors[i];
		
		for(j=0; j < g->terminals_n; j++)
		{
			sym = terminals[j];

			/* Si la conexión termina en un terminal, y el emisor
			 * no está marcado, añadir el emisor */
			if((con->sym == sym) && !NODE_IS_MARKED(con->from, MARK_PASS))
			{
				/* TODO: Add sym->from to the list */
				#warning "TODO: Create a GOOD list"
				
				list = realloc(++list_n * sizeof(*list));
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
	

}
