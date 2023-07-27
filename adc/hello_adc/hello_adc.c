/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int main() {
    #ifndef PICO_DEFAULT_LED_PIN
    #warning blink example requires a board with a regular LED
    #else
        const uint LED_PIN = PICO_DEFAULT_LED_PIN;
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);
 
        stdio_init_all();
        printf("ADC Example, measuring GPIO26\n");

        adc_init();

        // Make sure GPIO is high-impedance, no pullups etc
        adc_gpio_init(26);
        // Select ADC input 0 (GPIO26)
        adc_select_input(0);

        while (1) {
            // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
            //#define N 256
            uint32_t sum = 0;
            uint8_t i;
            for(i = 0; i < 32; ++i)
            {
                sum += adc_read();
            }
            uint16_t result = (uint16_t)(sum / 32);
            const float conversion_factor = 3.258f / (1 << 11);

            printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
            gpio_put(LED_PIN, 1);
            sleep_ms(250);
            gpio_put(LED_PIN, 0);
            sleep_ms(1750);
        }
    #endif
}
