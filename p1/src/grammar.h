#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NODE_VAR	0
#define NODE_CON	1
#define NODE_TER	2

struct symbol_t
{
	char type;	/* Tipo de nodo */
	char *name;	/* Nombre asignado al símbolo */
};

struct connector_t
{
	char type;	/* Tipo conector */
	void *from;	/* De donde viene la conexion */
	void *to1;	/* A donde va 1 */
	void *to2;	/* A donde va 2 */
};

struct grammar_t
{
	/* Conexiones */
	struct connector_t **connectors;
	long connectors_n;

	/* Símbolos no-terminales */
	struct symbol_t **variables;
	long variables_n;

	/* Símbolos terminales */
	struct symbol_t **terminals;
	long terminals_n;
};

#endif	/* GRAMMAR_H */
