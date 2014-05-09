#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_TRANS	(2*1024)
#define MAX_TAPE	(2*1024)
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

void msg(char *s)
{
	printf("%s\n", s);
}

void machine_halt()
{
	msg("Halting machine");
	exit(0);
}

void machine_init(struct machine_t *m)
{
	memset(m, 0, sizeof(struct machine_t));
}

int machine_position(int position, char direction)
{
	if(direction == 'L') position--;
	else if(direction == 'R') position++;

	if((position < 0) || (position >= MAX_TAPE))
	{
		machine_halt();
	}
	return position;
}

void machine_status(struct machine_t *m)
{
	int i=0;
	while(i < m->pos)
	{
		if(m->tape[i] != SYM_EMPTY)
		{
			printf("%c\t", m->tape[i]);
		}
		i++;
	}
	printf("s%d\t", m->st_now);
	while(i < MAX_TAPE)
	{
		if(m->tape[i] != SYM_EMPTY)
		{
			printf("%c\t", m->tape[i]);
		}
		i++;
	}
	printf("\n");
}

int machine_step(struct machine_t *m)
{
	int i;
	char actual = m->tape[m->pos];
	struct trans_t *trans;

	if (m->st_now == m->st_end) return 0;

	for(i=0; i < m->n_trans; i++)
	{
		trans = &(m->trans[i]);

		if((m->st_now == trans->st_r) && (actual == trans->r))
		{
			//printf("A-st_now=%d\ttape[pos]=%c\tpos=%d\n", m->st_now, m->tape[m->pos], m->pos);
			m->st_now = trans->st_w;
			m->tape[m->pos] = trans->w;
			m->pos = machine_position(m->pos, trans->dir);
			//printf("B-st_now=%d\ttape[pos]=%c\tpos=%d\n", m->st_now, m->tape[m->pos], m->pos);
			//printf("TAPE: %s\n", &(m->tape[POS_INIT]));
			machine_status(m);
			break;
		}
	}

	return 1;
}

void machine_transition_add(struct machine_t *m,
	int st_r, char r, int st_w, char w, char dir)
{
	if(m->n_trans == MAX_TRANS) return;

	struct trans_t *trans;
	
	trans = &(m->trans[m->n_trans]);
	trans->st_r = st_r;
	trans->r = r;
	trans->st_w = st_w;
	trans->w = w;
	trans->dir = dir;

	m->n_trans++;
}

int machine_main(int argc, char *argv[])
{
	struct machine_t machine;
	struct machine_t *m = &machine;

	machine_init(m);

	machine_transition_add(m,0,'a',  1,'a','N');
	machine_transition_add(m,0,'\0',  8,'\0','N');

	machine_transition_add(m,1,'a',  2,'A','R');
	machine_transition_add(m,1,'A',  1,'A','R');
	machine_transition_add(m,1,'B',  5,'B','R');

	machine_transition_add(m,2,'a',  2,'a','R');
	machine_transition_add(m,2,'b',  3,'B','R');
	machine_transition_add(m,2,'B',  2,'B','R');
	machine_transition_add(m,2,'C',  6,'C','R');

	machine_transition_add(m,3,'b',  3,'b','R');
	machine_transition_add(m,3,'c',  4,'C','R');
	machine_transition_add(m,3,'C',  3,'C','R');

	machine_transition_add(m,4,'>',  1,'>','R');
	machine_transition_add(m,4,'a',  4,'a','L');
	machine_transition_add(m,4,'A',  4,'A','L');
	machine_transition_add(m,4,'b',  4,'b','L');
	machine_transition_add(m,4,'B',  4,'B','L');
	machine_transition_add(m,4,'c',  4,'c','L');
	machine_transition_add(m,4,'C',  4,'C','L');
	machine_transition_add(m,4,'\0',  4,'\0','L'); //FIXME

	machine_transition_add(m,5,'B',  5,'B','R');
	machine_transition_add(m,5,'C',  6,'C','R');

	machine_transition_add(m,6,'C',  6,'C','R');
	machine_transition_add(m,6,'\0',  7,'\0','L');

	machine_transition_add(m,7,'>',  8,'>','R');
	machine_transition_add(m,7,'A',  7,'\0','L');
	machine_transition_add(m,7,'B',  7,'\0','L');
	machine_transition_add(m,7,'C',  7,'\0','L');

	m->st_init = 0;
	m->st_now = 0;
	m->st_end = 8;

	memcpy(&(m->tape[POS_INIT]), ">aaabbbccc", 10);
	m->pos = POS_INIT+1;
	
	//printf("TAPE: %s\n", &(m->tape[POS_INIT]));
	//printf("st_now=%d\ttape[pos]=%c\tpos=%d\n", m->st_now, m->tape[m->pos], m->pos);

	machine_status(m);
	while(machine_step(m));

	msg("Terminó");

	return 0;
}
