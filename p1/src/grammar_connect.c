#include "grammar_connect.h"
#include "grammar.h"
#include "list.h"
#include "dbg.h"


/*
 * Connector example:
 *
 *   ? --> (C) --> ?
 *          |
 *          v
 *         (?)
 */

/*
 * Connects from a symbol to connector.
 *
 * Before:
 *
 *   SRC     (C) --> ?
 *            |
 *            v
 *           (?)
 *
 * After:
 *
 *   SRC --> (C) --> ?
 *            |
 *            v
 *           (?)
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
 * Before:
 *
 *   (SRC)     (C) --> ?
 *              |
 *              v
 *             (?)
 *
 * After:
 *
 *   (SRC) --> (C) --> ?
 *              |
 *              v
 *             (?)
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
 * Before:
 *
 *   ? --> (C)     DST
 *          |
 *          v
 *         (?)
 *
 * After:
 *
 *   ? --> (C) --> DST
 *          |
 *          v
 *         (?)
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
 * Before:
 *
 *   ? --> (C) --> ?
 *          
 *          
 *        (DST)
 *
 * After:
 *
 *   ? --> (C) --> ?
 *          |
 *          v
 *        (DST)
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

/*
 * Disconnects a connector from a symbol.
 *
 * Before:
 *
 *   SRC --> (C) --> ?
 *            |
 *            v
 *           (?)
 *
 * After:
 *
 *   SRC     (C) --> ?
 *            |
 *            v
 *           (?)
 *
 */
int grammar_disconnect_from_symbol(struct connector_t *connector)
{
#ifndef NDEBUG
	if(connector->from == NULL) debug("connector->from == NULL");
#endif
	struct symbol_t *from;
	from = connector->from;
	return_if(list_remove(&(from->to), connector), -1);
	connector->from = NULL;
	return 0;
}

/*
 * Disconnects a connector from a connector.
 *
 * Before:
 *
 *   (SRC) --> (C) --> ?
 *              |
 *              v
 *             (?)
 *
 * After:
 *
 *   (SRC)     (C) --> ?
 *              |
 *              v
 *             (?)
 *
 */
void grammar_disconnect_from_connector(struct connector_t *connector)
{
#ifndef NDEBUG
	if(connector->from == NULL) debug("connector->from == NULL");
#endif
	struct connector_t *from;
	from = connector->from;
	from->con = NULL;
	connector->from = NULL;
}

/*
 * Disconnects a connector from any.
 *
 * Before:
 *
 *   ??? --> (C) --> ?
 *            |
 *            v
 *           (?)
 *
 * After:
 *
 *   ???     (C) --> ?
 *            |
 *            v
 *           (?)
 *
 */
int grammar_disconnect_from(struct connector_t *connector)
{
#ifndef NDEBUG
	if(connector->from == NULL) debug("connector->from == NULL");
#endif
	if(NODE_IS_TYPE(connector->from, NODE_CON))
	{
		grammar_disconnect_from_connector(connector);
		return 0;
	}
	else
	{
		return grammar_disconnect_from_symbol(connector);
	}
}

/*
 * Disconnects a connector to another connector.
 *
 * Before:
 *
 *   ? --> (C) --> ?
 *          |
 *          v
 *        (DST)
 *
 * After:
 *
 *   ? --> (C) --> ?
 *          
 *          
 *        (DST)
 *
 */
void grammar_disconnect_to_connector(struct connector_t *connector)
{
#ifndef NDEBUG
	if(connector->con == NULL) debug("connector->con == NULL");
#endif
	connector->con->from = NULL;
	connector->con = NULL;
}
/*
 * Disconnects a connector to a symbol.
 *
 * Before:
 *
 *   ? --> (C) --> DST
 *          |
 *          v
 *         (?)
 *
 * After:
 *
 *   ? --> (C)     DST
 *          |
 *          v
 *         (?)
 *
 */
int grammar_disconnect_to_symbol(struct connector_t *connector)
{
#ifndef NDEBUG
	if(connector->sym == NULL) debug("connector->sym == NULL");
#endif
	return_if(list_remove(&(connector->sym->from), connector), -1);
	connector->sym = NULL;
	return 0;
}

/*
 * Disconnects a connector from all.
 *
 * Before:
 *
 *   A --> (C) --> D
 *          |
 *          v
 *         (B)
 *
 * After:
 *
 *   A     (C)     D
 *          
 *          
 *         (B)
 *
 */
int grammar_disconnect_all(struct connector_t *connector)
{
	if(connector->from)
		return_if(grammar_disconnect_from(connector), -1);
	if(connector->sym)
		return_if(grammar_disconnect_to_symbol(connector), -1);
	if(connector->con)
		grammar_disconnect_to_connector(connector);

	return 0;
}
