#include "grammar_connect.h"
#include "grammar.h"
#include "list.h"
#include "dbg.h"


/*
 * Connects from a symbol to connector.
 *
 *  SRC -=-> (C) -...->
 *            |
 *            .
 *            .
 *            .
 *            |
 *            v
 */
int grammar_connect_from_symbol(struct connector_t *connector, struct symbol_t *src)
{
#ifndef NDEBUG
	if(connector->from != NULL) debug("connector->from != NULL");
#endif
	return_if(list_add(&(src->to), (void *) connector), -1);
	connector->from = src;
	return 0;
}

/*
 * Connects from a connector to a connector.
 *
 * ...-> (SRC) -=-> (C) -...->
 *         |        |
 *         .        .
 *         .        .
 *         .        .
 *         |        |
 *         v        v
 *
 */
void grammar_connect_from_connector(struct connector_t *connector, struct connector_t *src)
{
#ifndef NDEBUG
	if(src->con != NULL) debug("src->con != NULL");
	if(connector->from != NULL) debug("connector->from != NULL");
#endif
	src->con = connector;
	connector->from = src;
}

/*
 * Connects from a connector to a symbol.
 *
 *  ...-> (C) -=-> DST
 *         |
 *         .
 *         .
 *         .
 *         |
 *         v
 *
 */
int grammar_connect_to_symbol(struct connector_t *connector, struct symbol_t *dst)
{
#ifndef NDEBUG
	if(connector->sym != NULL) debug("connector->sym != NULL");
#endif
	return_if(list_add(&(dst->from), (void *) connector), -1);
	connector->sym = dst;
	return 0;
}

/*
 * Connects from a connector to a connector.
 *
 *  ...-> (C) -=-> (SRC) -...->
 *         |         |
 *         .         .
 *         .         .
 *         .         .
 *         |         |
 *         v         v
 *
 */
void grammar_connect_to_connector(struct connector_t *connector, struct connector_t *dst)
{
#ifndef NDEBUG
	if(connector->con != NULL) debug("connector->con != NULL");
	if(dst->from != NULL) debug("dst->from != NULL");
#endif
	dst->from = connector;
	connector->con = dst;
}


int grammar_disconnect_from(struct connector_t *connector, struct symbol_t *ptr)
{
	struct connector_t *c;
	struct symbol_t *s;

#ifndef NDEBUG
	if(connector->from == NULL) debug("connector->from == NULL");
#endif
	if(NODE_IS_TYPE(connector->from, NODE_CON))
	{
		c = (struct connector_t *)connector->from;
		/* TODO: REMOVE RECURSIVELLY ALL CONNECTORS */
		connector->from
	}
	return_if(list_add(&(src->to), (void *) connector), -1);
	connector->from = src;
	return 0;
}
int grammar_disconnect_symbol(struct connector_t *connector, struct symbol_t *src)
int grammar_disconnect_connector(struct connector_t *connector, struct symbol_t *src)
