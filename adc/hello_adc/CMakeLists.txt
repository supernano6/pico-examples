add_executable(hello_adc
        hello_adc.c
        )

target_link_libraries(hello_adc pico_stdlib hardware_adc)

# disable uart output, enable usb output
pico_enable_stdio_uart(hello_adc 0)
pico_enable_stdio_usb(hello_adc 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_adc)

# add url via pico_set_program_url
example_auto_set_url(hello_adc)
