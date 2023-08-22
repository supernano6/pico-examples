/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

/* Example code to talk to a sgm41511 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on sgm41511 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on sgm41511 board
   3.3v (pin 36) -> VCC on sgm41511 board
   GND (pin 38)  -> GND on sgm41511 board
*/

#define SGM_DEVICE_ADDR  0x6B
#define REG00_ADDR       0x00
#define REG01_ADDR       0x01
#define REG02_ADDR       0x02
#define REG03_ADDR       0x03
#define REG04_ADDR       0x04
#define REG05_ADDR       0x05
#define REG06_ADDR       0x06
#define REG07_ADDR       0x07
#define REG08_ADDR       0x08
#define REG09_ADDR       0x09
#define REG0A_ADDR       0x0A
#define REG0B_ADDR       0x0B

// Devices address
static int addr = SGM_DEVICE_ADDR;

#ifdef i2c_default
static void sgm41511_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {REG0B_ADDR, 0x80};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void sgm41511_read_raw(uint8_t *reg0B) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];
    int8_t retval;
    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = REG0B_ADDR;
    i2c_write_blocking(i2c_default, addr, &val, 1, false); // true to keep master control of bus
    retval = i2c_read_blocking(i2c_default, addr, buffer, 1, false);
    printf("%d\n", retval);

    *reg0B = buffer[0];
}
#endif

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN) || !defined(PICO_DEFAULT_LED_PIN)
    #warning i2c/sgm41511_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    printf("Hello, sgm41511! Reading raw data from registers...\n");

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    sleep_ms(10); // I2C初始化后等待10ms，再操作IIC
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    // sgm41511_reset();
    uint8_t reg0B;

    while (1) {
        sgm41511_read_raw(&reg0B);

        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        printf("Z = %d\n", reg0B);
        gpio_put(LED_PIN, 1);
        sleep_ms(50);
        gpio_put(LED_PIN, 0);
        sleep_ms(50);
    }

#endif
    return 0;
}
