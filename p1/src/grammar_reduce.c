#include "grammar_reduce.h"
#include "grammar.h"


int grammar_reduce_queue_check(struct queue_t queue, void *ptr)
{
	struct connector_t *connector;

	if(NODE_IS_TYPE(ptr, NODE_CON))
	{
		connector = (struct connector_t *) ptr;
		if(NODE_IS_MARKED(connector->sym, MARK_PASS))
		{
			if(!connector->con)
			{
				return 1;
			}
			else if(NODE_IS_MARKED(connector->con))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/* TODO: Review this code */
int grammar_reduce_no_generators(struct grammar_t *g)
{
	struct queue_t gen;
	struct list_node_t *list_node;
	void *tmp, *last;
	struct symbol_t *sym;

	queue_empty(&gen);
	
	/* Get all terminals */
	list_node = g->terminals.start
	while(list_node)
	{
		NODE_MARK(list_node->ptr, MARK_PASS);
		return_if(queue_push(&gen, list_node->ptr), -1);
		list_node = list_node->next;
	}

	return_if(!gen.start, -1);

	/* For each terminal, find all generators */
	do
	{
		
		tmp = gen.start->ptr;
		if(NODE_IS_TYPE(tmp, NODE_CON))
		{
			//conn stuff
		}
		else
		{
			sym = (struct symbol_t *) tmp;
			tmp = sym->from.start;
			/* Find connections to the symbol */
			while(tmp)
			{
				if(!NODE_IS_MARKED(tmp->ptr, MARK_PASS))
				{
					return_if(queue_push(&queue, tmp->ptr), -1);
					NODE_MARK(tmp->ptr, MARK_PASS);
				}
				tmp = tmp->next;
			}
		}
	}
	while(tmp != gen.start->ptr)
	return 0;
}
