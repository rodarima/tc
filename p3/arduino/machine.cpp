#include <string.h>
#include <stdlib.h>

#include <machine.h>

void msg(char *s)
{
	//printf("%s\n", s);
}

void machine_halt()
{
	msg("Halting machine");
	//exit(0);
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
	/*
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
	*/
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
			return 1;
		}
	}

	return -1;
}

char machine_char(struct machine_t *m)
{
	if(m) return m->tape[m->pos];
	return 0;
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

void machine_example(struct machine_t *m)
{
	machine_transition_add(m,0,0x01,  1,0x01,'N');
	machine_transition_add(m,0,0x00,  8,0x00,'N');

	machine_transition_add(m,1,0x01,  2,0x04,'R');
	machine_transition_add(m,1,0x04,  1,0x04,'R');
	machine_transition_add(m,1,0x05,  5,0x05,'R');

	machine_transition_add(m,2,0x01,  2,0x01,'R');
	machine_transition_add(m,2,0x02,  3,0x05,'R');
	machine_transition_add(m,2,0x05,  2,0x05,'R');
	machine_transition_add(m,2,0x06,  6,0x06,'R');

	machine_transition_add(m,3,0x02,  3,0x02,'R');
	machine_transition_add(m,3,0x03,  4,0x06,'R');
	machine_transition_add(m,3,0x06,  3,0x06,'R');

	machine_transition_add(m,4,0x07,  1,0x07,'R');
	machine_transition_add(m,4,0x01,  4,0x01,'L');
	machine_transition_add(m,4,0x04,  4,0x04,'L');
	machine_transition_add(m,4,0x02,  4,0x02,'L');
	machine_transition_add(m,4,0x05,  4,0x05,'L');
	machine_transition_add(m,4,0x03,  4,0x03,'L');
	machine_transition_add(m,4,0x06,  4,0x06,'L');
	machine_transition_add(m,4,0x00,  4,0x00,'L');

	machine_transition_add(m,5,0x05,  5,0x05,'R');
	machine_transition_add(m,5,0x06,  6,0x06,'R');

	machine_transition_add(m,6,0x06,  6,0x06,'R');
	machine_transition_add(m,6,0x00,  7,0x00,'L');

	machine_transition_add(m,7,0x07,  8,0x07,'R');
	machine_transition_add(m,7,0x04,  7,0x00,'L');
	machine_transition_add(m,7,0x05,  7,0x00,'L');
	machine_transition_add(m,7,0x06,  7,0x00,'L');

	m->st_init = 0;
	m->st_now = 0;
	m->st_end = 8;

	//memcpy(&(m->tape[POS_INIT]), "\x07\x01\x01\x01\x02\x02\x02\x03\x03\x03", 10);
#define N 4
	int i;
	m->tape[POS_INIT] = 0x07;
	for (i=0; i<N; i++)
	{
		m->tape[POS_INIT +1 + i] = 0x01;
		m->tape[POS_INIT +1 + i + N] = 0x02;
		m->tape[POS_INIT +1 + i + 2*N] = 0x03;
	}
	m->pos = POS_INIT+1;
	
	//printf("TAPE: %s\n", &(m->tape[POS_INIT]));
	//printf("st_now=%d\ttape[pos]=%c\tpos=%d\n", m->st_now, m->tape[m->pos], m->pos);

	//machine_status(m);
	//while(machine_step(m));
}
