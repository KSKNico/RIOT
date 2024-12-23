#include "tm1637.h"

#include "periph/gpio.h"
#include "ztimer.h"

#define DATA_COMMAND 0x40
#define DISPLAY_AND_CONTROL_COMMAND 0x80
#define ADDRESS_COMMAND 0xC0

#define ON_BIT 0x08

#define BIT_TIME_MS 5

static const uint8_t segments_array[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
};

static const uint8_t minus_sign = 0b01000000;

static void delay(void) {
    ztimer_sleep(ZTIMER_MSEC, BIT_TIME_MS);
}

static void start(tm1637_t const *dev) {
    gpio_write(dev->params.dio, false);
    delay();
}

static void stop(tm1637_t const *dev) {
    gpio_write(dev->params.dio, false);
    delay();
    gpio_write(dev->params.clk, true);
    delay();
    gpio_write(dev->params.dio, true);
    delay();
}

static void transmit_byte(tm1637_t const *dev, uint8_t byte) {
    for (int i = 0; i < 8; ++i) {
        bool value = (byte >> i) & 0x01;
        gpio_write(dev->params.clk, false);
        delay();
        gpio_write(dev->params.dio, value);
        delay();
        gpio_write(dev->params.clk, true);
        delay();
    }

    // we do not read the ACK as it is not necessary for normal functionality
    gpio_write(dev->params.clk, false);
    gpio_write(dev->params.dio, true);
    delay();
    gpio_write(dev->params.clk, true);
    delay();
    gpio_write(dev->params.clk, false);
    delay();
}

int tm1637_init(tm1637_t *dev, const tm1637_params_t *params) {
    assert(params != NULL);

    assert(params->brightness <= 0x0f);

    dev->params = *params;

 
    if (gpio_init(dev->params.clk, GPIO_OUT)) {
        return -1;
    }

    if (gpio_init(dev->params.dio, GPIO_OUT)) {
        return -1;
    }

    gpio_write(dev->params.clk, false);
    gpio_write(dev->params.dio, false);

    return 0;
}

static void transmit_segments(const tm1637_t *dev, const uint8_t *segments) {
    start(dev);
    transmit_byte(dev, DATA_COMMAND);
    stop(dev);

    start(dev);
    transmit_byte(dev, ADDRESS_COMMAND);

    for (int i = 0; i < 4; ++i) {
        transmit_byte(dev, segments[i]);
    }
    stop(dev);

    start(dev);
    transmit_byte(dev, DISPLAY_AND_CONTROL_COMMAND | dev->params.brightness | ON_BIT);

    stop(dev);
}

void tm1637_write_number(const tm1637_t *dev, int16_t number) {
    assert(number <= 9999);
    assert(number >= -999);
    // get each digit of the number

    uint8_t segments[4] = {0, 0, 0, 0};

    if (number < 0) {
        segments[0] = 0b01000000;
        number = -number;
    }

    for (int i = 0; i < 4; ++i) {
        segments[3 - i] = segments_array[number % 10];
        number /= 10;

        if (number == 0) {
            segments[3 - i] = 0;
        }
    }

    transmit_segments(dev, segments);
}