#ifndef _GRAMMAR_REDUCE_H_
#define _GRAMMAR_REDUCE_H_

#include "grammar.h"

int grammar_reduce_no_generators(struct grammar_t *g);
int grammar_reduce_unreachables(struct grammar_t *g);
int grammar_reduce_e_productions(struct grammar_t *g);
int grammar_reduce_unitary(struct grammar_t *g);



#endif /* _GRAMMAR_REDUCE_H_ */
