#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NODE_GEN	0
#define NODE_CON	1
#define NODE_TER	2

struct node_symbol_t
{
	char type;	/* Tipo de nodo */
	char *name;	/* Nombre asignado al símbolo */
};

struct node_connector_t
{
	char type;	/* Tipo conector */
	void *from;	/* De donde viene la conexion */
	void *to_1;	/* A donde va 1 */
	void *to_2;	/* A donde va 2 */
};

struct grammar_t
{
	/* Conexiones */
	struct node_connector_t *connectors;
	size_t connectors_n;

	/* Símbolos generadores */
	struct node_symbol_t *generators;
	size_t generators_n;

	/* Símbolos terminales */
	struct node_symbol_t *terminals;
	size_t terminals_n;
};

#endif	/* GRAMMAR_H */
