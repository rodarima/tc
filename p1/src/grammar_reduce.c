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
	struct symbol_t *symbol;
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
		debug("Fatal: There is not valid start symbol");
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

int grammar_reduce_e_productions_mark(struct grammar_t *g, struct symbol_t *epsilon)
{
	struct queue_t nullable;
	struct list_node_t *list_node;
	struct symbol_t *symbol;

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
#if 0
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

/* TODO: Add to grammar.c */
int grammar_connector_duplicate(struct grammar_t *g,
	struct connector_t *c, struct connector_t **clone)
{
	//struct connector_t *connector;
	struct symbol_t *symbol;

	return_if(grammar_connector_new(g, clone), -1);
	
	if(NODE_IS_TYPE(c->from, NODE_CON))
	{
		//connector = (struct connector_t *) c->from;
		(*clone)->from = c->from;
		//grammar_connect_from_connector(*clone, connector);
	}
	else
	{
		symbol = (struct symbol_t *) c->from;
		debug("Symbol = %p", symbol);
		return_if(grammar_connect_from_symbol(*clone, symbol), -1);
	}
	return_if(grammar_connect_to_symbol(*clone, c->sym), -1);
	if(c->con)
	{
		(*clone)->con = c->con;
		//grammar_connect_to_connector(*clone, c->con);
	}
	return 0;
}

int grammar_reduce_e_productions_connector_from(struct grammar_t *g,
	struct symbol_t *s, struct connector_t *c, void **side)
{
	struct connector_t *tmp;

	if(NODE_IS_TYPE(c->from, NODE_CON))
	{
		debug("First duplicate %p", c->from);
		return_if(grammar_connector_duplicate(g, c->from, &tmp), -1);
		debug("%p is now at %p too", c->from, tmp);
		tmp->con = NULL;
		*side = tmp;
		c = tmp;
	}
	else
	{
		*side = c->from;
		return 0;
	}
	while(NODE_IS_TYPE(c->from, NODE_CON))
	{
		debug("Duplicating %p", c->from);
		return_if(grammar_connector_duplicate(g, c->from, &tmp), -1);
		debug("%p is now at %p too", c->from, tmp);
		c->from = tmp;
		tmp->con = c;

		c = c->from;
	}
	return 0;
}

int grammar_reduce_e_productions_connector_to(struct grammar_t *g,
	struct symbol_t *s, struct connector_t *c, void *side)
{
	struct connector_t *tmp, *from;

	if(c->con)
	{
		debug("First duplicate %p", c->con);
		return_if(grammar_connector_duplicate(g, c->con, &tmp), -1);
		debug("%p is now at %p too", c->con, tmp);
		if(NODE_IS_TYPE(side, NODE_CON))
		{
			grammar_connect_from_connector(tmp, side);
		}
		else
		{
			return_if(grammar_connect_from_symbol(tmp, side), -1);
		}
		//tmp->from = side;
		c = tmp;
	}
	else
	{
		if(NODE_IS_TYPE(side, NODE_CON))
		{
			from = (struct connector_t *) side;
			from->con = NULL;
			//return_if(grammar_connect_to_symbol(from, c->sym), -1);
		}
		return 0;
	}
	while(c->con)
	{
		debug("Duplicating %p", c->con);
		return_if(grammar_connector_duplicate(g, c->con, &tmp), -1);
		debug("%p is now at %p too", c->con, tmp);
		grammar_connect_to_connector(c, tmp);

		c = tmp;
	}
	return 0;
}

int grammar_reduce_e_productions_connector(struct grammar_t *g,
	struct symbol_t *s, struct connector_t *c)
{
	void *side;

	/* XXX: If fails, then grammar becomes inestable */
	return_if(grammar_reduce_e_productions_connector_from(g, s, c, &side), -1);
	return_if(grammar_reduce_e_productions_connector_to(g, s, c, side), -1);
	return 0;
}

int grammar_reduce_e_productions_symbol(struct grammar_t *g, struct symbol_t *s)
{
	struct list_node_t *node, *end;
	struct connector_t *connector;

	node = s->from.start;
	end = s->from.end;
	while(node)
	{
		connector = (struct connector_t *) node->ptr;
		debug("Symbol %s connector(%p)", s->name, connector);
		if(grammar_reduce_e_productions_connector(g, s, connector))
		{
			return -1;
		}

		if(node == end) break;
		node = node->next;
	}
	return 0;
}

int grammar_reduce_e_productions_remove_epsilon(struct grammar_t *g, 
	struct symbol_t *epsilon)
{
	struct list_node_t *node, *next;
	struct connector_t *connector, *tmp, *tmp2;
	struct symbol_t *symbol;
	long deleted = 0;

	node = epsilon->from.start;
	while(node)
	{
		next = node->next;
		connector = (struct connector_t *) node->ptr;
		if(!connector->con)
		{
			if(!NODE_IS_TYPE(connector->from, NODE_CON))
			{
				// A->e
				return_if(grammar_disconnect_from_symbol(connector), -1);
				return_if(grammar_disconnect_to_symbol(connector), -1);
			}
			else
			{
				// A->o->e
				return_if(grammar_disconnect_to_symbol(connector), -1);
				grammar_disconnect_from_connector(connector);
			}
		}
		else
		{
			if(!NODE_IS_TYPE(connector->from, NODE_CON))
			{
				//A->e
				symbol = (struct symbol_t *) connector->from;
				tmp = connector->con;
				return_if(grammar_disconnect_to_symbol(connector), -1);
				return_if(grammar_disconnect_from_symbol(connector), -1);
				grammar_disconnect_to_connector(connector);
				return_if(grammar_connect_from_symbol(tmp, symbol), -1);
			}
			else
			{
				//A-o->e
				tmp = (struct connector_t *) connector->from;
				tmp2 = (struct connector_t *) connector->con;
				return_if(grammar_disconnect_to_symbol(connector), -1);
				grammar_disconnect_from_connector(connector);
				grammar_disconnect_to_connector(connector);
				grammar_connect_to_connector(tmp, tmp2);

			}
		}
		return_if(list_remove(&(g->connectors), connector), -1);
		grammar_connector_free(connector);
		deleted++;
		node = next;
	}

	printf("Removed %ld null productions\n", deleted);
	return 0;
}

int grammar_reduce_e_productions_add_start(struct grammar_t *g, 
	struct symbol_t *epsilon)
{
	struct connector_t *connector;

	if(!g->start)
	{
		debug("No start symbol defined");
		return -1;
	}
	if(NODE_IS_MARKED(g->start, MARK_NULL))
	{
		return_if(grammar_connector_new(g, &connector), -1);
		return_if(grammar_connect_from_symbol(connector, g->start), -1);
		return_if(grammar_connect_to_symbol(connector, epsilon), -1);
	}
	return 0;
}

int grammar_reduce_e_productions_remove(struct grammar_t *g)
{
	struct symbol_t *symbol;
	struct list_node_t *node;

	node = g->variables.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		if(NODE_IS_MARKED(symbol, MARK_NULL))
		{
			debug("Symbol %s (%p)", symbol->name, symbol);
			if(grammar_reduce_e_productions_symbol(g, symbol))
			{
				return -1;
			}
		}
		node = node->next;
	}
	
	return 0;
}

int grammar_reduce_e_productions_find_epsilon(struct grammar_t *g, 
	struct symbol_t **epsilon)
{
	if(!(*epsilon = list_find(&(g->terminals), 
		(void *) SYM_EPSILON, grammar_cmp_str_symbol)))
	{
		debug("There is no ∈-productions (epsilon-productions)");
		return -1;
	}
	return 0;
}

int grammar_reduce_e_productions(struct grammar_t *g)
{
	struct symbol_t *epsilon;
	return_if(grammar_reduce_e_productions_find_epsilon(g, &epsilon), 0);
	return_if(grammar_reduce_e_productions_mark(g, epsilon), -1);
	return_if(grammar_reduce_e_productions_remove(g), -1);
	return_if(grammar_reduce_e_productions_remove_epsilon(g, epsilon), -1);
	return_if(grammar_reduce_e_productions_add_start(g, epsilon), -1);
	grammar_reduce_unmark(g);
	return 0;
}

int grammar_reduce_is_unitary(struct connector_t *c)
{
	if(NODE_IS_TYPE(c->from, NODE_CON))
	{
		return 0;
	}
	if(c->con)
	{
		return 0;
	}
	if(NODE_IS_TYPE(c->sym, NODE_TER))
	{
		return 0;
	}
	return 1;
}

int grammar_reduce_unitary_duplicate_path(struct grammar_t *g,
	struct symbol_t *s, struct connector_t *c)
{
	struct connector_t *tmp, *tmp2;

	debug("Duplicating path from %p to symbol %s (%p)", c, s->name, s);

	return_if(grammar_connector_new(g, &tmp), -1);

	return_if(grammar_connect_from_symbol(tmp, s), -1);
	return_if(grammar_connect_to_symbol(tmp, c->sym), -1);
	debug("First connector from %s (%p) now is %p to symbol %s (%p)", 
		s->name, s, tmp, tmp->sym->name, tmp->sym);

	c = c->con;
	while(c)
	{
		return_if(grammar_connector_new(g, &tmp2), -1);
		grammar_connect_from_connector(tmp2, tmp);
		return_if(grammar_connect_to_symbol(tmp2, c->sym), -1);
		
		debug("New connector from %s (%p) now is %p to symbol %s (%p)", 
			s->name, s, tmp2, tmp2->sym->name, tmp2->sym);
		
		tmp = tmp2;

		c = c->con;
	}

	return 0;
}
	
int grammar_reduce_unitary_connector(struct grammar_t *g, struct connector_t *c)
{
	struct symbol_t *from, *symbol;
	struct connector_t *connector;
	struct list_node_t *node, *node2/*, *end*/;

	from = (struct symbol_t *) c->from;
	symbol = c->sym;

	node = symbol->to.start;
	//end = symbol->to.end;
	while(node)
	{
		node2 = node->next;
		connector = node->ptr;
		/* Prevent loops */
		/* Prevent A=A */
		debug("Trying connector %p", connector);
		if((connector->sym != from) || (connector->con))
		{
			debug("%s != %s", connector->sym->name, from->name);
			if((!connector->con) && NODE_IS_MARKED(connector->sym, MARK_UNIT))
			{
				debug("Loop detected with %s (%p)", connector->sym->name, connector->sym);
			}
			else
			{
				if(grammar_reduce_unitary_duplicate_path(g, from, connector))
				{
					return -1;
				}
			}
		}

		/*
		if(end == node)
		{
			debug("Original end reached, breaking");
			break;
		}
		*/
		node = node2;
	}
	
	return 0;
}

int grammar_reduce_unitary_symbol(struct grammar_t *g, struct symbol_t *symbol, long *deleted)
{
	struct list_node_t *node, *node2, *next;
	struct connector_t *connector;

	node = symbol->to.start;
	while(node)
	{
		connector = (struct connector_t *) node->ptr;
		/* All new nodes will be added to the end */
		if(grammar_reduce_is_unitary(connector) && (!NODE_IS_MARKED(connector->sym, MARK_UNIT)))
		{
			debug("Deleting unitary connection %p %s->%s",
				connector, symbol->name, connector->sym->name);
			
			debug("Marking %s (%p)", connector->sym->name, connector->sym);
			NODE_MARK(connector->sym, MARK_UNIT);

			grammar_reduce_unitary_connector(g, connector);
			/* Delete connection */
			*deleted++;
			grammar_disconnect_all(connector);
			if(!symbol->to.start)
			{
				debug("WARNING, variable %s isolated", symbol->name);
			}
			grammar_connector_free(connector);
			return_if(list_remove(&(g->connectors), connector), -1);
			//grammar_rules_print(g);
		}

		node = node->next;
	}
	return 0;
}

int grammar_reduce_unitary(struct grammar_t *g)
{
	struct list_node_t *node, *next;
	struct symbol_t *from, *symbol;
	struct connector_t *connector;
	long deleted = 0;

	node = g->variables.start;
	while(node)
	{
		symbol = (struct symbol_t *) node->ptr;
		return_if(grammar_reduce_unitary_symbol(g, symbol, &deleted), -1);
		grammar_reduce_unmark(g);
		node = node->next;
	}
	printf("Removed %ld unitary productions\n", deleted);
	
	return 0;
}
