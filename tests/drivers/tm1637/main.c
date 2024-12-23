#include "tm1637.h"

#include "periph/gpio.h"
#include <stdio.h>
#include "ztimer.h"

static bool test_gpio_read(gpio_t pin) {
    return gpio_read(pin);
}

/*
static bool test_numbers(tm1637_t *dev) {
    tm1637_write_number(dev, 0);
    return true;
}
*/

int main(void) {
    tm1637_params_t tm1637_params = {
        .clk = GPIO_PIN(1, 14),
        .dio = GPIO_PIN(1, 13),
        .brightness = 0x01
    };

    tm1637_t dev;

    tm1637_init(&dev, &tm1637_params);
    

    for (int i = 9999; i >= -999 ; --i) {
        tm1637_write_number(&dev, 0, false, false);
    }


}