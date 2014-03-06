#ifndef PARSE_STD_H
#define PARSE_STD_H

#include <stdio.h>
#include "grammar.h"

char *equal_position(const char *str);
int is_std_rule(const char *str);
void parse_std_left(const char *src, char *dst);
void parse_std_right(const char *src, char *dst);
int is_std_rule_left(const char *src);
int is_std_rule_right(const char *src);
int parse_std_add_rule(const char *rule, struct grammar_t *grammar);


#endif /* PARSE_STD_H */
