#include "grammar_reduce.h"
#include "grammar.h"
#include "queue.h"
#include "list.h"
#include "dbg.h"


int is_connector_complete(struct connector_t *connector)
{
	if(NODE_IS_MARKED(connector->sym, MARK_GEN))
	{
		if( (!connector->con) || NODE_IS_MARKED(connector->con, MARK_GEN) )
		{
			return 1;
		}
	}
	return 0;
}

/* TODO: Review this code */
int grammar_reduce_no_generators(struct grammar_t *g)
{
	struct queue_t gen;
	struct list_node_t *list_node;
	void *last, *element, *from;
	struct symbol_t *symbol;
	struct connector_t *connector;

	queue_empty(&gen);
	
	/* Get all terminals */
	list_node = g->terminals.start;
	while(list_node)
	{
		debug("Adding %p\n", list_node->ptr);
		NODE_MARK(list_node->ptr, MARK_GEN);
		if(queue_push(&gen, list_node->ptr))
		{
			queue_clear(&gen);
			return -1;
		}
		last = list_node->ptr;
		list_node = list_node->next;
	}

	return_if(!gen.start, 0);

	/* For each terminal, find all generators */
	do
	{
		if(queue_pop(&gen, &element))
		{
			debug("ERROR FATAL");
		}

		if(NODE_IS_TYPE(element, NODE_CON))
		{
			if(NODE_IS_MARKED(element, MARK_GEN))
			{
				debug("This cannot happen %p", element);
			}
			if(!NODE_IS_MARKED(element, MARK_QUEUED))
			{
				debug("This cannot happen %p", element);
			}
			connector = (struct connector_t *) element;
			if(is_connector_complete(connector))
			{
				NODE_MARK(element, MARK_GEN);
				if(!IS_NODE_MARKED(connector->from, MARK_GEN))
				{
					if(!IS_NODE_TYPE(connector->from, NODE_CON))
					{
						NODE_MARK(connector->from, MARK_GEN);
					}
					if(!IS_NODE_MARKED(connector->from, MARK_QUEUED))
					{
						NODE_MARK(connector->from, MARK_QUEUED);
						if(queue_push(&gen, connector->from))
						{
							queue_clear(&gen);
							return -1;
						}
						last = connector->from;
					}
				}
			}
			else if(last == element)
			{
				break;
			}
			else
			{
				if(queue_push(&gen, element))
				{
					queue_clear(&gen);
					return -1;
				}
			}
		}
		else
		{
			symbol = (struct symbol_t *) element;

			list_node = symbol->from.start;
			/* Find connections to the symbol */
			while(list_node)
			{
				from = list_node->ptr;
				if(NODE_IS_MARKED(from, MARK_GEN))
				{
					debug("This cannot happen %p", from);
				}
				if(!NODE_IS_MARKED(from, MARK_QUEUED))
				{
					if(queue_push(&gen, from))
					{
						queue_clear(&gen);
						return -1;
					}
					NODE_MARK(from, MARK_QUEUED);
				}
				last = from;
				list_node = list_node->next;
			}
		}
	}
	while(gen.start);

	queue_clear(&gen);
	
	printf("Marked variables are:\n");
	list_node = g->variables.start;
	while(list_node)
	{
		symbol = (struct symbol_t *) list_node->ptr;
		if(NODE_IS_MARKED(symbol, MARK_GEN))
		{
			grammar_symbol_print(symbol);
		}
		list_node = list_node->next;
	}
	printf("Unmarked variables are:\n");
	list_node = g->variables.start;
	while(list_node)
	{
		symbol = (struct symbol_t *) list_node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_GEN))
		{
			grammar_symbol_print(symbol);
		}
		list_node = list_node->next;
	}


	return 0;
}
