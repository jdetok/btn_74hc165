#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../lib/lcd/lcd.h"

// #include <stdint.h>

#define INTLED (1 << PB5)

// inputs shift register 74HC165 pins
#define CE_PIN (1 << PD4) // D4 | chip pin 15 
#define DATA_PIN (1 << PD5) // D5 | chip pin 7
#define CLK_PIN (1 << PD6) // D6 | chip pin 2
#define PL_PIN (1 << PD7) // D7 | chip pin 1 | referred to as load sometimes

// // lcd shift register 74HC595 pins
// #define LCD_SER (1 << PB0)
// #define LCD_CLK (1 << PB1)
// #define LCD_LTC (1 << PB2)

static void hc165_pulse(volatile uint8_t *port, uint8_t pin) {
    *port |=  pin;
    *port &= ~pin;
}

uint8_t hc165_read(void) {
    uint8_t val = 0;

    // pl pulse, parallel inputs into shift register
    PORTD &= ~PL_PIN;
    _delay_us(1);
    PORTD |= PL_PIN;

    PORTD &= ~CE_PIN; // clock enable low (active)

    for (uint8_t i = 0; i < 8; i++) {
        // PORTD |= CLK_PIN; // clock in high

        val <<= 1; // left shift 1

        if (PIND & DATA_PIN) {
            val |= 1; // shift in a 1 to most sig bit (rightmost)
        }
        
        // pulse clock
        PORTD |= CLK_PIN;
        _delay_us(1);
        PORTD &= ~CLK_PIN;    
    }
    PORTD |= CE_PIN; // disable clock

    return ~val;
}

typedef struct {
    uint8_t sr_pos;
    uint8_t state;
    char *name;
} button;

typedef enum {
    SH0,
    SH1,
    SH2,
    SH3,
    SH4,
    SH5,
    SH6,
    SH7,
    BTN_CNT // number of buttons (8)
} btn_id;

typedef struct {
    button btn[8];
    uint8_t state;
} buttons;

int main() {
    // DDRB |= LCD_SER | LCD_CLK | LCD_LTC; // lcd sr output pins
    DDRB |= INTLED;
    DDRD |= CE_PIN | CLK_PIN | PL_PIN; // input sr output pins
    DDRD &= ~DATA_PIN; // data in as input

    buttons btns = {
        .btn = {
            {.sr_pos = (1 << SH0), .state = 0, .name = "t1"},
            {.sr_pos = (1 << SH1), .state = 0, .name = "t2"},
            {.sr_pos = (1 << SH2), .state = 0, .name = "t3"},
            {.sr_pos = (1 << SH3), .state = 0, .name = "t4"},
            {.sr_pos = (1 << SH4), .state = 0, .name = "t5"},
            {.sr_pos = (1 << SH5), .state = 0, .name = "t6"},
            {.sr_pos = (1 << SH6), .state = 0, .name = "t7"},
            {.sr_pos = (1 << SH7), .state = 0, .name = "t8"}
        },
        .state = 0
    };

    lcd_init();
    // lcd_clr_print(0, 0, "test");

    while (1) {
        uint8_t state = hc165_read();

        if (state != btns.state) {
            char binval[9];
            for (int b = 7; b >= 0; b--) {
                binval[7 - b] = (state & (1 << b)) ? '1' : '0';
            }
            // itoa(hc165_read(), binval, 2);
            binval[8] = '\0';
            lcd_goto_print(0, 0, binval);

            for (uint8_t i = SH0; i < BTN_CNT; i++) {    
                if (state & (btns.btn[i].sr_pos) && !(btns.state & (btns.btn[i].sr_pos))) {
                    btns.btn[i].state ^= 1;
                    lcd_goto_print(1, 0, btns.btn[i].name);
                    lcd_goto_print(1, 3, "state: ");
                    char buf[2];
                    itoa(btns.btn[i].state, buf, 10);
                    lcd_goto_print(1, 9, buf);
                }
                // write to byte   
            }
            btns.state = state;
        }
    }
}
// while (1) {
//         PORTB |= INTLED;
//         _delay_ms(100);
//         PORTB &= ~INTLED; 
//         _delay_ms(100);
//     }