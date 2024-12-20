#ifndef TM1637_H
#define TM1637_H

#include "board.h"

typedef struct {
    gpio_t clk;
    gpio_t dio;
} tm1637_params_t;

typedef struct {
    tm1637_params_t params;
} tm1637_t;

int tm1637_init(tm1637_t *dev, const tm1637_params_t *params);

void tm1637_turn_on();

void tm1637_turn_off();

void tm1637_write_number(int number);

void tm1637_write_hex(int hex);

void tm1637_turn_on_colon();

void tm1637_turn_off_colon();


#endif 