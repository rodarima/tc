#include "list.h"

#include <stdlib.h>

/* Make a list empty */
void list_empty(struct list_t *list)
{
	list->start = NULL;
	list->end = NULL;
}

/* Delete all elements in a list */
void list_clear(struct list_t *list)
{
	while(list->start)
	{
		list_remove(list, list->start);
	}
}

/* Create a new node, with 'ptr' contents */
int list_node_init(struct list_node_t **node_addr, void *ptr)
{
	struct list_node_t *_node_addr;

	_node_addr = malloc(sizeof(struct list_node_t));
	if(!_node_addr) return -1;

	_node_addr->prev = NULL;
	_node_addr->next = NULL;
	_node_addr->ptr = ptr;

	*node_addr = _node_addr;
	return 0;
}

/* Free a node, NOT the contents */
void list_node_free(struct list_node_t *node)
{
	free(node);
}

/* Adds a already created node to the end of the list */
void list_add(struct list_t *list, struct list_node_t *node)
{
	node->prev = list->end;
	node->next = NULL;
	if(list->end)
	{
		list->end->next = node;
	}
	list->end = node;
	if(!list->start)
	{
		list->start = node;
	}
}

/* Remove node from the list */
void list_remove(struct list_t *list, struct list_node_t *node)
{
	if(node->prev)
	{
		node->prev->next = node->next;
	}
	else if(list->start == node)
	{
		list->start = node->next;
	}

	if(node->next)
	{
		node->next->prev = node->prev;
	}
	else if(list->end == node)
	{
		list->end = node->prev;
	}
	
	list_node_free(node);
}
