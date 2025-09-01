#include "lcd.h"

// pulse helper
static void sr_pulse(volatile uint8_t *port, uint8_t pin) {
    *port |=  pin;
    *port &= ~pin;
}

// shift out one byte MSB first
static void sr_shiftout(uint8_t data) {
    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i)) {
            PORTB |= LCD_SER;
        } else {
            PORTB &= ~(LCD_SER);
        }
        sr_pulse(&PORTB, LCD_CLK); // pulse clock
    }
    sr_pulse(&PORTB, LCD_LTC); // pulse latch
}

// ===== LCD internals =====
static void lcd_send_nibble(uint8_t nib, uint8_t rs) {
    uint8_t out = (nib << 4);
    if (rs) out |= (1 << 0);  // RS on Q0
    out |= (1 << 2);          // EN on Q2

    sr_shiftout(out);
    _delay_us(1);
    sr_shiftout(out & ~(1 << 2));
    _delay_us(50);
}

static void lcd_send(uint8_t val, uint8_t rs) {
    lcd_send_nibble(val >> 4, rs);
    lcd_send_nibble(val & 0x0F, rs);
}

// ===== Public API =====
void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
    if (cmd == 0x01 || cmd == 0x02)
        _delay_ms(2);
}

void lcd_data(uint8_t data) {
    lcd_send(data, 1);
}

void lcd_print(const char *str) {
    while (*str) lcd_data(*str++);
}

void lcd_goto(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0 ? 0x00 : 0x40) + col;
    lcd_command(0x80 | addr);
}

void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

// clear, goto, print
void lcd_clr_print(uint8_t row, uint8_t col, const char *str) {
    lcd_clear();
    lcd_goto(row, col);
    lcd_print(str);
}
void lcd_goto_print(uint8_t row, uint8_t col, const char *str) {
    lcd_goto(row, col);
    lcd_print(str);
}

void lcd_init(void) {
    // configure pins as outputs
    // DDRB   |= (1 << SR_SER_PIN);
    // SR_LATCH_DDR |= (1 << SR_LATCH_PIN);
    // SR_CLK_DDR   |= (1 << SR_CLK_PIN);
    DDRB |= LCD_SER | LCD_CLK | LCD_LTC; // lcd sr output pins

    _delay_ms(40);

    lcd_send_nibble(0x03, 0);
    _delay_ms(5);
    lcd_send_nibble(0x03, 0);
    _delay_us(150);
    lcd_send_nibble(0x03, 0);
    lcd_send_nibble(0x02, 0);

    lcd_command(0x28); // 4-bit, 2-line
    lcd_command(0x0C); // display on
    lcd_command(0x01); // clear
    _delay_ms(2);
    lcd_command(0x06); // entry mode

    lcd_clear();
}
