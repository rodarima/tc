#ifndef _TURING_H_
#define _TURING_H_
/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
*/
#define MAX_TRANS	(1*128)
#define MAX_TAPE	(1*512)
#define POS_INIT	(MAX_TAPE/2)
#define SYM_EMPTY	'\0'

struct trans_t
{
	int st_r;
	char r;

	int st_w;
	char w;
	char dir;
};

struct machine_t
{
	struct trans_t trans[MAX_TRANS];
	int n_trans;

	int st_init;
	int st_now;
	int st_end;
	
	char tape[MAX_TAPE];
	int pos;
};

void machine_halt();
void machine_init(struct machine_t *m);
int machine_position(int position, char direction);
void machine_status(struct machine_t *m);
int machine_step(struct machine_t *m);
void machine_transition_add(struct machine_t *m, int st_r, char r, int st_w, char w, char dir);
void machine_example(struct machine_t *m);
char machine_char(struct machine_t *m);


#endif
