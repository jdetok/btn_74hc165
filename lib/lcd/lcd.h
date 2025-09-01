// lcd.h
#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>

// lcd shift register 74HC595 pins
#define LCD_SER (1 << PB0)
#define LCD_CLK (1 << PB1)
#define LCD_LTC (1 << PB2)

// // SER (data) = D6 = PD6
// #define LCD_SER_PORT PORTD
// #define LCD_SER_DDR    DDRD
// #define LCD_SER_PIN    PD6

// // CLK = D8 = PB0
// #define LCD_CLK_PORT  PORTD
// #define LCD_CLK_DDR    DDRD
// #define LCD_CLK_PIN    PD7

// #define LCD_LATCH_PIN  PB0

// LCD API
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_print(const char *str);
void lcd_goto(uint8_t row, uint8_t col);
void lcd_clear(void);
void lcd_clr_print(uint8_t row, uint8_t col, const char *str);
void lcd_goto_print(uint8_t row, uint8_t col, const char *str);
#endif
