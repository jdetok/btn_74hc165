#include <avr/io.h>
#include <util/delay.h>
// #include <stdint.h>

#define INTLED (1 << PB5)

int main() {
    DDRB |= INTLED;

    while (1) {
    PORTB |= INTLED;
    _delay_ms(100);
    PORTB &= ~INTLED; 
    _delay_ms(100);
    }
}