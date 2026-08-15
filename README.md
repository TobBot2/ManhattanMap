# Manhattan Map Project

This project is for my wooden map of manhattan

## Configuration

TODO: Change lights configuration by setting config file and compiling the config file into a header file or something to be included.

### Wiring

Button pin is defined in `main.c`. Default pin 3.

Data pins for LEDs are set in `ManhattanMap.c` via function call. Default pins 6 & 7

I used a resistor on all data lines as well as a pull up resistor for the button (I think the rpi pico has one built in anyway though).

External power supply should have positive connected in parallel to both LED strips, and the ground should be connect to the LED strips and rpi pico in parallel. Rpi pico can be powered by up to 5.5 V, so powering the pico itself with the 5 V power supply should work (though maybe a capacitor or something would be good for voltage spikes).

## Resources

- [Raspberry Pi Pico datasheet](https://pip-assets.raspberrypi.com/categories/610-raspberry-pi-pico/documents/RP-008307-DS-2-pico-datasheet.pdf)
- [WS2812b datasheet](https://www.ledyilighting.com/wp-content/uploads/2025/02/WS2812B-datasheet.pdf)
- [Raspberry Pi Pico SDK documentation](https://www.raspberrypi.com/documentation/pico-sdk/)
