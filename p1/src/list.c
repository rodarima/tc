#include "list.h"

#include <stdlib.h>
#include "dbg.h"

/* Make a list empty */
void list_empty(struct list_t *list)
{
	list->start = NULL;
	list->end = NULL;
}

/* Delete all elements in a list */
void list_clear(struct list_t *list)
{
	struct list_node_t *node, *node2;

	node = list->start;
	while(node)
	{
		node2 = node->next;
		list_node_free(node);
		node = node2;
	}
	list_empty(list);
}

/* Delete all elements in a list, and apply func before */
void list_clear_func(struct list_t *list, void (*func)(void *))
{
	struct list_node_t *node, *node2;
	
	node = list->start;
	while(node)
	{
		node2 = node->next;
		(func)(node->ptr);
		list_node_free(node);
		node = node2;
	}
	list_empty(list);
}

/* Apply func to all elements in the list */
void list_map(struct list_t *list, void (*func)(void *))
{
	struct list_node_t *node;
	
	node = list->start;
	while(node)
	{
		(func)(node->ptr);
		node = node->next;
	}
}

/* Search a element in the list */
void *list_find(struct list_t *list, const void *ptr, int (*cmp)(const void *, const void *))
{
	struct list_node_t *node;
	
	node = list->start;
	while(node)
	{
		if((cmp)(ptr, node->ptr) == 0) return node->ptr;
		node = node->next;
	}
	return NULL;
}

/* Search a node in the list */
struct list_node_t *list_find_node(struct list_t *list, const void *ptr, int (*cmp)(const void *, const void *))
{
	struct list_node_t *node;
	
	node = list->start;
	while(node)
	{
		if((cmp)(ptr, node->ptr) == 0) return node;
		node = node->next;
	}
	return NULL;
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
void list_node_add(struct list_t *list, struct list_node_t *node)
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

/* Create a new node, and adds it to the list */
int list_add(struct list_t *list, void *ptr)
{
	struct list_node_t *node;
	
	return_if(list_node_init(&node, ptr), -1);
	list_node_add(list, node);

	return 0;
}

/* Remove node from the list */
void list_remove_node(struct list_t *list, struct list_node_t *node)
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

/* Remove a element from the list */
int list_remove(struct list_t *list, void *ptr)
{
	struct list_node_t *node;
	
	node = list->start;
	while(node)
	{
		if(node->ptr == ptr)
		{
			list_remove_node(list, node);
			return 0;
		}
		node = node->next;
	}
	return -1;
}

