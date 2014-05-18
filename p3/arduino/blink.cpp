#include <Arduino.h>
#include <alpha.h>
#include <machine.h>

int led1 = 5, led2 = 6, led3 = 7;

#define MAX_LEDS	7
#define MAX_BUTTONS	4
#define MAX_TICKS_BUTTON 100
#define MAX_CHAR	16

#define BUTTON_LEFT	0x01
#define BUTTON_CENTER	0x02
#define BUTTON_RIGHT	0x04
#define BUTTON_OK	0x08

byte seven_seg_digits[10][7] =
{
	{ 1,1,1,1,1,1,0 },  // = 0
	{ 0,1,1,0,0,0,0 },  // = 1
	{ 1,1,0,1,1,0,1 },  // = 2
	{ 1,1,1,1,0,0,1 },  // = 3
	{ 0,1,1,0,0,1,1 },  // = 4
	{ 1,0,1,1,0,1,1 },  // = 5
	{ 1,0,1,1,1,1,1 },  // = 6
	{ 1,1,1,0,0,0,0 },  // = 7
	{ 1,1,1,1,1,1,1 },  // = 8
	{ 1,1,1,0,0,1,1 }   // = 9
};

byte hex_str[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};



//int leds[MAX_LEDS] = { 2, 3, 4, 5, 10, 11, 12, 13 };


int leds[MAX_LEDS] = { 11,13,10,12,5,3,4 };
int dot_led = 2;
int buttons[MAX_BUTTONS] = { 9, 8, 7, 6 };
struct machine_t machine;
struct machine_t *m = &machine;

void setup()
{
	Serial.begin(9600);
	int i;
	for (i=0; i<MAX_LEDS; i++)
	{
		pinMode(leds[i], OUTPUT);
	}
	for (i=0; i<MAX_BUTTONS; i++)
	{
		pinMode(buttons[i], INPUT);
	}
	pinMode(dot_led, OUTPUT);

	machine_init(m);
	machine_example(m);
}

void _put(byte *b)
{
	int i;
	for (i=0; i<7; i++)
	{
		digitalWrite(leds[i], b[i]);
	}
}
void put(int c)
{
	_put(hex_seg[c]);
}

int char_next(int c, int d)
{
	int t = c + d;
	while(t<0) t+=MAX_CHAR;
	return t%MAX_CHAR;
}

void clear()
{
	int i;
	for (i=0; i<7; i++)
	{
		digitalWrite(leds[i], LOW);
	}
}

void beep(int led, int time)
{
	digitalWrite(led, HIGH);
	delay(time);
	digitalWrite(led, LOW);
}

int read_buttons()
{
	int status = 0;
	int i;
	
	for (i=0; i<MAX_BUTTONS; i++)
	{
		status |= digitalRead(buttons[i]) << i;
	}
	return status;
}

int buttons_last_st = 0;
int ticks_button = 0;

void insert()
{
	int buttons_st;
	digitalWrite(dot_led, HIGH);
	
	int insert_mode = 1;
	
	while(read_buttons());
	delay(200);

	while(insert_mode)
	{
		buttons_st = read_buttons();
		if(buttons_st)
		{
			if(buttons_st & BUTTON_CENTER)
			{
				break;
			}
			if(buttons_st & BUTTON_LEFT)
			{
				m->tape[m->pos] = char_next(m->tape[m->pos], -1);
			}
			if(buttons_st & BUTTON_RIGHT)
			{
				m->tape[m->pos] = char_next(m->tape[m->pos], +1);
			}

			put(machine_char(m));
			digitalWrite(dot_led, LOW);
			delay(200);
			digitalWrite(dot_led, HIGH);
		}
		buttons_last_st = buttons_st;
	}

	digitalWrite(dot_led, LOW);
}

void machine_status_arduino(struct machine_t *m)
{
	int i=0;
	while(i < m->pos)
	{
		if(m->tape[i] != SYM_EMPTY)
		{
			Serial.print(m->tape[i], 16);
			Serial.print('\t');
			//printf("%c\t", m->tape[i]);
		}
		i++;
	}
	//printf("s%d\t", m->st_now);
	Serial.print('s');
	Serial.print(m->st_now);
	Serial.print('\t');
	while(i < MAX_TAPE)
	{
		if(m->tape[i] != SYM_EMPTY)
		{
			Serial.print(m->tape[i], 16);
			Serial.print('\t');
			//printf("%c\t", m->tape[i]);
		}
		i++;
	}
	Serial.println();
}

int wheel_pos = 0;
void wheel()
{
	wheel_pos = (wheel_pos+1) % 6;
	_put(alpha_seg[19 + wheel_pos]);
	delay(20);
}

void run_turing()
{
	int i=0,but,step;
	while(read_buttons());
	delay(200);
	while((!(read_buttons() & BUTTON_OK)) && (1==(step=machine_step(m))))
	{
		machine_status_arduino(m);
		//if(++i>1000)
		{
			wheel();
			i=0;
		}
	}
	if(step == 0)
	{
		put(1);
	}
	else if(step == -1)
	{
		put(0);
	}
	delay(300);
	while(!read_buttons());
	if(read_buttons() & BUTTON_OK)
	{
		m->st_now = m->st_init;
	}
}

void loop()
{
	int buttons_st;

	put(machine_char(m));

	buttons_st = read_buttons();
	if(buttons_st)
	{
		if(buttons_st & BUTTON_CENTER)
		{
			insert();
			//tape[index] = (tape[index]+1) % 10;
		}
		if(buttons_st & BUTTON_LEFT)
		{
			m->pos = (m->pos+MAX_TAPE-1) % MAX_TAPE;
		}
		if(buttons_st & BUTTON_RIGHT)
		{
			m->pos = (m->pos+1) % MAX_TAPE;
		}
		if(buttons_st & BUTTON_OK)
		{
			Serial.println("Goooooooooo!");
			run_turing();
		}

		put(machine_char(m));
		beep(dot_led, 100);
		delay(100);
	}
	buttons_last_st = buttons_st;
}
