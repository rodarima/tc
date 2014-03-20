#define NDEBUG

#include "grammar_reduce.h"
#include "grammar.h"
#include "grammar_connect.h"
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
int grammar_reduce_no_generators_mark(struct grammar_t *g)
{
	struct queue_t gen;
	struct list_node_t *list_node;
	void *last, *element, *from;
	struct symbol_t *symbol;
	struct connector_t *connector;
	long iteration = 0;

	queue_empty(&gen);
	
	/* Get all terminals */
	list_node = g->terminals.start;
	while(list_node)
	{
		debug("Adding terminal %p", list_node->ptr);
		NODE_MARK(list_node->ptr, MARK_GEN);
		if(queue_push(&gen, list_node->ptr))
		{
			queue_clear(&gen);
			return -1;
		}
		last = list_node->ptr;
		debug("last = %p", last);
		list_node = list_node->next;
	}

	return_if(!gen.start, 0);
	debug("Okay, let's go");

	/* For each terminal, find all generators */
	do
	{
		debug("---- Iteration %ld ----", iteration);
		if(queue_pop(&gen, &element))
		{
			debug("ERROR FATAL");
		}
		debug("Popped %p", element);

		if(NODE_IS_TYPE(element, NODE_CON))
		{
			debug("Type NODE_CON");
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
				debug("It's complete, adding MARK_GEN");
				NODE_MARK(element, MARK_GEN);
				if(!IS_NODE_MARKED(connector->from, MARK_GEN))
				{
					debug("'from' is not marked as MARK_GEN");
					if(!IS_NODE_TYPE(connector->from, NODE_CON))
					{
						debug("'from' is not NODE_CON, marking as MARK_GEN");
						NODE_MARK(connector->from, MARK_GEN);
					}
					if(!IS_NODE_MARKED(connector->from, MARK_QUEUED))
					{
						debug("'from' is not marked as MARK_QUEUED, marking");
						NODE_MARK(connector->from, MARK_QUEUED);
						debug("Pushing 'from'");
						if(queue_push(&gen, connector->from))
						{
							queue_clear(&gen);
							return -1;
						}
						debug("Last = 'from'");
						last = connector->from;
					}
					else
					{
						if(gen.end)
						{
							last = gen.end->ptr;
						}
					}
				}
				else
				{
					if(gen.end)
					{
						last = gen.end->ptr;
					}
				}
			}
			else if(last == element)
			{
				debug("last == element");
				break;
			}
			else
			{
				debug("Adding connector to the queue, another time");
				if(queue_push(&gen, element))
				{
					queue_clear(&gen);
					return -1;
				}
				debug("last = %p", last);
			}
		}
		else
		{
			debug("Type symbol");
			symbol = (struct symbol_t *) element;

			list_node = symbol->from.start;
			/* Find connections to the symbol */
			debug("Listing symbol froms");
			while(list_node)
			{
				debug("Node %p", list_node->ptr);
				from = list_node->ptr;
				if(NODE_IS_MARKED(from, MARK_GEN))
				{
					debug("This cannot happen %p", from);
				}
				if(!NODE_IS_MARKED(from, MARK_QUEUED))
				{
					debug("Node %p is not marked as MARK_QUEUED, pushing", from);
					if(queue_push(&gen, from))
					{
						queue_clear(&gen);
						return -1;
					}
					debug("Marking as MARK_QUEUED");
					NODE_MARK(from, MARK_QUEUED);
				}
				debug("last = %p", from);
				last = from;
				list_node = list_node->next;
			}
		}
		iteration++;
	}
	while(gen.start);

	queue_clear(&gen);
	
#if 0
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
#endif

	return 0;
}

int grammar_reduce_no_generators_remove(struct grammar_t *g)
{
	struct connector_t *connector;
	struct symbol_t *symbol;
	struct list_node_t *node, *node2;
	struct queue_t orph;
	void *element;
	long deleted = 0;

	queue_empty(&orph);

	node = g->connectors.start;
	while(node)
	{
		node2 = node->next;
		connector = (struct connector_t *) node->ptr;
		if(!NODE_IS_MARKED(connector, MARK_GEN))
		{
			//FIXME: return OK?
			if(connector->con)
			{
				if(NODE_IS_MARKED(connector->con, MARK_GEN))
				{
					queue_push(&orph, connector->con);
				}
			}
			return_if(grammar_disconnect_all(connector), -1);
			grammar_connector_free(connector);
			list_remove_node(&(g->connectors), node);
			deleted++;
		}
		node = node2;
	}

	while(!queue_pop(&orph, &element))
	{
		debug("Popped %p", element);
		connector = (struct connector_t *) element;
		if(connector->con)
		{
			if(!NODE_IS_MARKED(connector->con, MARK_GEN))
			{
				//FIXME: return & mem
				debug("FATAL, connector %p have no-generator connector->con connection", connector);
				queue_clear(&orph);
				return -1;
			}

			debug("Pushing connector->con %p", connector->con);
			if(queue_push(&orph, connector->con))
			{
				queue_clear(&orph);
				return -1;
			}
		}
		return_if(grammar_disconnect_all(connector), -1);
		grammar_connector_free(connector);
		//TODO: Try to reduce this O(n) deletion
		list_remove(&(g->connectors), connector);
		deleted++;
	}

	node = g->variables.start;
	while(node)
	{
		node2 = node->next;
		symbol = (struct symbol_t *) node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_GEN))
		{
			if(symbol->to.start)
			{
				debug("FATAL, symbol %s have to connections", symbol->name);
				return -1;
			}
			if(symbol->from.start)
			{
				debug("FATAL, symbol %s have from connections", symbol->name);
				return -1;
			}
			grammar_symbol_free(symbol);
			list_remove_node(&(g->variables), node);
			deleted++;
		}
		node = node2;
	}
	printf("Removed %ld elements no-generators\n", deleted);

	return 0;
}

void grammar_reduce_no_generators_unmark(struct grammar_t *g)
{
	struct list_node_t *node;
	struct symbol_t *symbol;
	struct connector_t *connector;

	node = g->connectors.start;
	while(node)
	{
		connector = (struct connector_t *) node->ptr;
		NODE_UNMARK(connector);
		node = node->next;
	}
	node = g->variables.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		NODE_UNMARK(symbol);
		node = node->next;
	}
	node = g->terminals.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		NODE_UNMARK(symbol);
		node = node->next;
	}
}

int grammar_reduce_no_generators(struct grammar_t *g)
{
	return_if(grammar_reduce_no_generators_mark(g), -1);
	return_if(grammar_reduce_no_generators_remove(g), -1);
	grammar_reduce_no_generators_unmark(g);
	return 0;
}

#undef NDEBUG
