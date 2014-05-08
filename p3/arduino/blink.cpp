#include <Arduino.h>

int led1 = 5, led2 = 6, led3 = 7;

#define MAX_LEDS	8
#define MAX_BUTTONS	4

int leds[MAX_LEDS] = { 2, 3, 4, 5, 10, 11, 12, 13 };
int buttons[MAX_BUTTONS] = { 6, 7, 8, 9 };

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
}

void on(int led, int time)
{
	int i;
	for (i=0; i<MAX_LEDS; i++)
	{
		digitalWrite(leds[i], LOW);
	}
	digitalWrite(leds[led], HIGH);
	delay(time);
}

void st()
{
	int status[MAX_BUTTONS] = {0,0,0,0};
	int i;
	for (i=0; i<MAX_BUTTONS; i++)
	{
		status[i] = digitalRead(buttons[i]);
		Serial.print(status[i]);
		Serial.print(" ");
	}
	Serial.println();
}


void loop()
{
	int i;
	for (i=0; i<MAX_LEDS; i++)
	{
		on(i, 200);
		st();
	}
}
