#include <MIDI.h>
#include <SoftwareSerial.h>
#include <avr/io.h>

#define RX 4
#define TX 5
#define LED 0			//	PB0
#define STARTSTOP 1		//	PB1
#define CLOCK 3			//	PB3
#define CLOCKPULSE 5000	//	microseconds
#define LEDPULSE 7000	//	microseconds

SoftwareSerial mSerial(RX, TX); // Only receive used. TX mapped to unused pin.
MIDI_CREATE_INSTANCE(SoftwareSerial, mSerial, MIDI);

long timer = 0;
bool running = false;
long now;
bool LEDRunning = false;
int counter;
long endPulseTime;
int LEDFlashDivisor = 4;	//	Flash the LED every x clocks.

void setup()
{
	pinMode(LED, OUTPUT);
	pinMode(STARTSTOP, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(RX, INPUT);

	//	Handlers
	MIDI.setHandleClock(handleClock);
	MIDI.setHandleContinue(handleContinue);
	MIDI.setHandleStart(handleStart);
	MIDI.setHandleStop(handleStop);

	MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

	//	Flash LED to show everything's okay
	digitalWrite(LED, HIGH);
	delay(100);
	digitalWrite(LED, LOW);
	MIDI.read();
}

void loop()
{
	// Read incoming messages
	//MIDI.read();

	//	This should be okay because any delays happen after the rising edge
	if (running)
	{
		now = micros();
		if (now >= endPulseTime) {
			endClock();
			if (LEDRunning)
			{
				endLED();
				LEDRunning = false;
			}
			running = false;
		}
	}

	MIDI.read();
}

void handleClock()
{
	//if (running) { endClock; }

	PORTB |= (1 << PB3);   // PB3 high

	timer = micros();
	endPulseTime = micros() + CLOCKPULSE;
	counter++;
	if (counter > LEDFlashDivisor) { counter = 1; }
	if (counter == 1) {
		PORTB |= (1 << PB0);   // PB0 high
		LEDRunning = true;
	}
	running = true;
}

void handleContinue()
{
	handleClock();
}

void handleStart()
{
	PORTB |= (1 << PB1);   // PB1 high
	counter = 0;
}

void handleStop()
{
	PORTB &= ~(1 << PB1);   // PB1 low
	counter = 0;
}

void endClock() {
	PORTB &= ~(1 << PB3);   // PB3 low
}

void endLED() {
	PORTB &= ~(1 << PB0);   // PB0 low
}