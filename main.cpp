#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

extern "C" {
    #include "nokia/nokia5110.h"
}

#define LED PB5
#define BUTTON PB3

#define BODS 7                   //BOD Sleep bit in MCUCR
#define BODSE 2                  //BOD Sleep enable bit in MCUCR

#define MAX_LED_DELAY 255

#define TURN_LED_ON() PORTB |=(1<<LED)
#define TURN_LED_OFF() PORTB &=~(1<<LED);


int volatile ledLevel = 0;

void setLedLevel(int level);
void powerOff();

// interrupt service routine
ISR(PCINT0_vect) {
    if(ledLevel < 255) {
        setLedLevel(255);
        // Disable interrupts
        cli();
        _delay_ms(100);
        // Enable interrupts
        sei();
        return;
    } else {
        setLedLevel(0);
        // Disable interrupts
        cli();
        _delay_ms(300);
        // Enable interrupts
        sei();
        powerOff();
    }
    return;
}

void setup() {
    _delay_ms(1000);
    // Set led pin to output
    DDRB |= (1 << LED);

    // Turn on pullup resistor
    PORTB |= (1 << BUTTON);

    // Set button pin to input
    DDRB &= ~(1 << BUTTON);

    // turn off ADC
    ADCSRA &= ~(1<<ADEN);
    // disable the analog comparator
    ACSR |= _BV(ACD);
    // turn off the brown-out detector
    MCUCR |= _BV(BODS) | _BV(BODSE); 

    // pin change mask: listen to portb bit 3
    PCMSK |= (1<<PCINT3);   
    // enable PCINT interrupt 
    GIMSK |= (1<<PCIE); 
    sei();

    setLedLevel(255);

    lcdInit();

    lcdPrint(0, 0, "0123456789", 2);

    // powerOff();
}

void powerOff() {
    // Disable interrupts
    cli();
    // do a complete power-down
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    // enable sleep mode
    sleep_enable();
    // allow interrupts to end sleep mode
    sei();
    // go to sleep
    sleep_cpu();
    // disable sleep mode for safety
    sleep_disable();
    // Disable interrupts
    cli();
}

void setLedLevel(int level) {
    if(ledLevel > MAX_LED_DELAY) {
        ledLevel = MAX_LED_DELAY;
    }
    ledLevel = level;
    if(ledLevel == 0) {
        TURN_LED_OFF();
    }
}

void loop() {
    if(ledLevel == 0) {
        return;
    }
    TURN_LED_ON();
    _delay_us(1);    
    TURN_LED_OFF();
    for(int i = 0; i < MAX_LED_DELAY - ledLevel; i++) {
        _delay_us(1);
    }
    // _delay_us(1);
}

int main(void) {
    setup();
    while(true) {
        loop();
    }
}