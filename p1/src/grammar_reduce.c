#include "grammar_reduce.h"
#include "grammar.h"
#include "grammar_connect.h"
#include "queue.h"
#include "list.h"
#include "dbg.h"


int is_connector_complete(struct connector_t *connector, char mark)
{
	if(NODE_IS_MARKED(connector->sym, mark))
	{
		if( (!connector->con) || NODE_IS_MARKED(connector->con, mark) )
		{
			return 1;
		}
	}
	return 0;
}

int grammar_reduce_mark_from(struct queue_t *queue, char mark)
{
	struct list_node_t *list_node;
	void *last, *element, *from;
	struct symbol_t *symbol;
	struct connector_t *connector;
	long iteration = 0;

	debug("Okay, let's go");
	last = queue->end->ptr;

	queue_print(queue);
	while(!queue_pop(queue, &element))
	{
		debug("---- Iteration %ld ----", iteration);
		queue_print(queue);
		debug("last = %p", last);

		if(NODE_IS_TYPE(element, NODE_CON))
		{
			debug("Connector(%p)", element);
			if(NODE_IS_MARKED(element, mark))
			{
				debug("This cannot happen %p", element);
			}
			if(!NODE_IS_MARKED(element, MARK_QUEUED))
			{
				debug("This cannot happen %p", element);
			}
			connector = (struct connector_t *) element;
			if(is_connector_complete(connector, mark))
			{
				debug("It's complete, adding mark");
				NODE_MARK(element, mark);
				if(!IS_NODE_MARKED(connector->from, mark))
				{
					debug("%p is not marked as mark", connector->from);
					if(!IS_NODE_TYPE(connector->from, NODE_CON))
					{
						debug("%p is not NODE_CON, marking as mark", connector->from);
						NODE_MARK(connector->from, mark);
					}
					if(!IS_NODE_MARKED(connector->from, MARK_QUEUED))
					{
						debug("%p is not marked as MARK_QUEUED, marking", connector->from);
						NODE_MARK(connector->from, MARK_QUEUED);
						debug("Pushing %p", connector->from);
						if(queue_push(queue, connector->from))
						{
							queue_clear(queue);
							return -1;
						}
						debug("Last = %p", connector->from);
						last = connector->from;
					}
					else
					{
						if(queue->end)
						{
							last = queue->end->ptr;
						}
					}
				}
				else
				{
					if(queue->end)
					{
						last = queue->end->ptr;
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
				if(queue_push(queue, element))
				{
					queue_clear(queue);
					return -1;
				}
				debug("last = %p", last);
			}
		}
		else
		{
			debug("Symbol(%p)", element);
			symbol = (struct symbol_t *) element;

			list_node = symbol->from.start;


			/* Find connections to the symbol */
			debug("Listing symbol froms");
			while(list_node)
			{
				debug("+- Node %p", list_node->ptr);
				from = list_node->ptr;
				/*
				if(NODE_IS_MARKED(from, mark))
				{
					debug("??? This cannot happen %p", from);
				}
				*/
				if(!NODE_IS_MARKED(from, MARK_QUEUED))
				{
					debug("   Node %p is not marked as MARK_QUEUED, pushing", from);
					if(queue_push(queue, from))
					{
						queue_clear(queue);
						return -1;
					}
					debug("   Marking as MARK_QUEUED");
					NODE_MARK(from, MARK_QUEUED);
				}
				debug("   last = %p", from);
				last = from;
				list_node = list_node->next;
			}
			
			if(last == symbol)
			{
				debug("Breaking last(%p) = symbol(%p)", last, symbol);
				break;
			}
		}
		iteration++;
	}
	return 0;
}

/* TODO: Review this code */
int grammar_reduce_no_generators_mark(struct grammar_t *g)
{
	struct queue_t gen;
	struct list_node_t *list_node;

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
		list_node = list_node->next;
	}

	return_if(!gen.start, 0);
	debug("Okay, let's go");

	/* For each terminal, find all generators */
	if(grammar_reduce_mark_from(&gen, MARK_GEN))
	{
		queue_clear(&gen);
		return -1;
	}

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
	
	if(!NODE_IS_MARKED(g->start, MARK_GEN))
	{
		debug("Removing no-generator start symbol");
		g->start = NULL;
	}

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
			if(symbol == g->start)
			{
				debug("FATAL: Start symbol being removed");
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

void grammar_reduce_unmark(struct grammar_t *g)
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
	grammar_reduce_unmark(g);
	return 0;
}


int grammar_reduce_unreachables_mark(struct grammar_t *g)
{
	struct queue_t reach;
	void *element;
	struct connector_t *connector;
	struct symbol_t *symbol;
	struct list_node_t *node;
	long iteration = 0;

	queue_empty(&reach);

	if(!g->start)
	{
		printf("Fatal: There is not valid start symbol\n");
		return -1;
	}

	/* Add first symbol */
	return_if(queue_push(&reach, g->start), -1);
	NODE_MARK(g->start, MARK_REACH);

	while(!queue_pop(&reach, &element))
	{
		debug("---- Iteration %ld ----", iteration);
		if(NODE_IS_TYPE(element, NODE_CON))
		{
			connector = (struct connector_t *) element;
			NODE_MARK(connector, MARK_REACH);
			if(connector->con && (!NODE_IS_MARKED(connector->con, MARK_REACH)))
			{
				if(queue_push(&reach, connector->con))
				{
					queue_clear(&reach);
					return -1;
				}
				NODE_MARK(connector->con, MARK_REACH);
			}
			if(NODE_IS_TYPE(connector->sym, NODE_VAR))
			{
				if(!NODE_IS_MARKED(connector->sym, MARK_REACH))
				{
					if(queue_push(&reach, connector->sym))
					{
						queue_clear(&reach);
						return -1;
					}
				}
			}
			NODE_MARK(connector->sym, MARK_REACH);
		}
		else
		{
			if(NODE_IS_TYPE(element, NODE_TER))
			{
				debug("This cannot happend.");
				return -1;
			}
			symbol = (struct symbol_t *) element;

			node = symbol->to.start;
			while(node)
			{
				element = node->ptr;

				if(!NODE_IS_MARKED(element, MARK_REACH))
				{
					if(queue_push(&reach, element))
					{
						queue_clear(&reach);
						return -1;
					}
					NODE_MARK(element, MARK_REACH);
				}
				
				node = node->next;
			}
		}
		iteration++;
	}
	
#if 0
	printf("Marked variables are:\n");
	node = g->variables.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		if(NODE_IS_MARKED(symbol, MARK_REACH))
		{
			grammar_symbol_print(symbol);
		}
		node = node->next;
	}
	printf("Marked terminals are:\n");
	node = g->terminals.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		if(NODE_IS_MARKED(symbol, MARK_REACH))
		{
			grammar_symbol_print(symbol);
		}
		node = node->next;
	}
	printf("Unmarked variables are:\n");
	node = g->variables.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_REACH))
		{
			grammar_symbol_print(symbol);
		}
		node = node->next;
	}
	printf("Unmarked terminals are:\n");
	node = g->terminals.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_REACH))
		{
			grammar_symbol_print(symbol);
		}
		node = node->next;
	}
#endif

	return 0;
}

int grammar_reduce_unreachables_remove(struct grammar_t *g)
{
	struct connector_t *connector;
	struct symbol_t *symbol;
	struct list_node_t *node, *node2;
	long deleted = 0;
	
	if(!NODE_IS_MARKED(g->start, MARK_REACH))
	{
		debug("Removing unreachable start symbol");
		g->start = NULL;
	}

	/* TODO: Use only var->to and var->from from unreachables */
	node = g->connectors.start;
	while(node)
	{
		node2 = node->next;
		connector = (struct connector_t *) node->ptr;
		if(!NODE_IS_MARKED(connector, MARK_REACH))
		{
			return_if(grammar_disconnect_all(connector), -1);
			grammar_connector_free(connector);
			list_remove_node(&(g->connectors), node);
			deleted++;
		}
		node = node2;
	}
	node = g->variables.start;
	while(node)
	{
		node2 = node->next;
		symbol = (struct symbol_t *) node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_REACH))
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
			if(symbol == g->start)
			{
				debug("FATAL: Start symbol being removed");
				return -1;
			}
			grammar_symbol_free(symbol);
			list_remove_node(&(g->variables), node);
			deleted++;
		}
		node = node2;
	}
	node = g->terminals.start;
	while(node)
	{
		node2 = node->next;
		symbol = (struct symbol_t *) node->ptr;
		if(!NODE_IS_MARKED(symbol, MARK_REACH))
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
			if(symbol == g->start)
			{
				debug("FATAL: Start symbol being removed");
				return -1;
			}
			grammar_symbol_free(symbol);
			list_remove_node(&(g->terminals), node);
			deleted++;
		}
		node = node2;
	}

	printf("Removed %ld unreachable elements\n", deleted);

	return 0;
}

int grammar_reduce_unreachables(struct grammar_t *g)
{
	return_if(grammar_reduce_unreachables_mark(g), -1);
	return_if(grammar_reduce_unreachables_remove(g), -1);
	grammar_reduce_unmark(g);
	return 0;
}

int grammar_reduce_e_productions_mark(struct grammar_t *g)
{
	struct queue_t nullable;
	struct list_node_t *list_node;
	struct symbol_t *symbol;
	void *epsilon;

	if(!(epsilon = list_find(&(g->terminals), (void *) SYM_EPSILON, grammar_cmp_str_symbol)))
	{
		printf("There is no ∈-productions (epsilon-productions)\n");
		return 0;
	}

	queue_empty(&nullable);
	NODE_MARK(epsilon, MARK_NULL);

	if(queue_push(&nullable, epsilon))
	{
		return -1;
	}
	
	if(grammar_reduce_mark_from(&nullable, MARK_NULL))
	{
		queue_clear(&nullable);
		return -1;
	}

	queue_clear(&nullable);
#if 1
	printf("Marked variables are:\n");
	list_node = g->variables.start;
	while(list_node)
	{
		symbol = (struct symbol_t *) list_node->ptr;
		if(NODE_IS_MARKED(symbol, MARK_NULL))
		{
			grammar_symbol_print(symbol);
		}
		list_node = list_node->next;
	}
#endif

	return 0;
}

int grammar_reduce_e_productions_remove(struct grammar_t *g)
{
	

	return 0;
}

int grammar_reduce_e_productions(struct grammar_t *g)
{
	return_if(grammar_reduce_e_productions_mark(g), -1);
	return 0;
}
