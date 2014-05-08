#include <Arduino.h>
#include <alpha.h>

int led1 = 5, led2 = 6, led3 = 7;

#define MAX_LEDS	7
#define MAX_BUTTONS	4
#define MAX_TICKS_BUTTON 100

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



//int leds[MAX_LEDS] = { 2, 3, 4, 5, 10, 11, 12, 13 };

int leds[MAX_LEDS] = { 11,13,10,12,5,3,4 };
int dot_led = 2;
int buttons[MAX_BUTTONS] = { 9, 8, 7, 6 };

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
}

void put(int c)
{
	byte *a = alpha_seg[c];

	int i;
	for (i=0; i<7; i++)
	{
		digitalWrite(leds[i], a[i]);
	}
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

int index = 0;
int buttons_last_st = 0;
int ticks_button = 0;
char tape[1024] = {0};

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
		if((buttons_st) && (buttons_st != buttons_last_st))
		{
			if(buttons_st & BUTTON_CENTER)
			{
				break;
			}
			if(buttons_st & BUTTON_LEFT)
			{
				tape[index] = (tape[index]+9) % 10;
			}
			if(buttons_st & BUTTON_RIGHT)
			{
				tape[index] = (tape[index]+1) % 10;
			}

			put(tape[index]);
			digitalWrite(dot_led, LOW);
			delay(200);
			digitalWrite(dot_led, HIGH);
		}
		buttons_last_st = buttons_st;
	}

	digitalWrite(dot_led, LOW);
}

void loop()
{
	int buttons_st;

	put(tape[index]);
	delay(1);

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
			index = (index+1024-1) % 1024;
		}
		if(buttons_st & BUTTON_RIGHT)
		{
			index = (index+1) % 1024;
		}
		Serial.println(buttons_st);

		beep(dot_led, 200);

		/* Multi click */
		//delay(200);
		/*clear();
		delay(100);
		put(tape[index]);
		delay(100);*/

	}
	buttons_last_st = buttons_st;
}
