# Manhattan Map Project

This project is for my wooden map of manhattan with individually addressable LEDs running along the subway lines.

This program runs on a raspberry pi pico to control the LEDs. A button input changes between display modes, e.g.
- solid colors
- wave pattern
- pulse pattern
- simulated trains

### Implementation

The code abstracts concepts in the following hierarchy (lower items know nothing about higher items):
- DisplayMode
- Map
- TrainLine
- Pixel
- config
- quickpix

DisplayMode serves as an overall controller of the colors based on its own internal state.

Map serves to represent the map of the train system by storing data on pixel positions in x-y space, which pixels correspond to which train lines, and which pixels are intersections where multiple train lines connect. It mostly handles connecting TrainLine and Pixel concepts together, allowing for navigation of the network and control over the pixels based on the map.

TrainLine keeps track of what pixels belong to a train line, as well as the reverse (what train lines belong to each pixel).

Pixel stores the abstract idea of a pixel in the context of this project (a LED with color at a position in space). In theory this could be reused for non-map related projects.

config is just configuration which has minimal dependencies (code that reads the config should parse values from primitives like ints and floats).

quickpix is a small wrapper around the hardware functions ws2812.pio provides. It simplifies the interface so it easily corresponds to physical layout (multiple chains of pixels), while also keeping it easy to access any given pixel by a single integer ID.

### Limitations/TODO

There are 1000 things to do still before this is a fully robust project, but some major things I wanted to do next that I don't have time to do include...
- Utilize hardware efficiencies better (DMA, render buffer, multicore processing)
- Better error handling/asserting valid configurations as well as debugging support.
- Automatic switching between small and large maps (change some internal implementation depending on number of pixels, train lines, etc.)
- Streamline the API to better reflect how the physical LEDs look/connect vs functions to control them.
- Restructure configs to be .yaml format. Parsed during build into a source file which gets read into source files to build module-specific config structs to pass into init functions.
- And of course much more...

## Configuration

Define a map configuration by setting `CONFIG` (extern variable in `config.h`). Example included with `Manhattan.c`.

### Wiring

Customizable with `CONFIG` variable (see above).

Suggested wiring:
- button pin 3,
- ws2812b pins starting at pin 6 and going up as needed (remember to keep data lines < 5 meters)

I used a resistor on all data lines as well as a pull up resistor for the button (I think the rpi pico has one built in anyway though).

External power supply should have positive connected in parallel to both LED strips, and the ground should be connect to the LED strips and rpi pico in parallel. Rpi pico can be powered by up to 5.5 V, so powering the pico itself with the 5 V power supply should work (though maybe a capacitor or something would be good for voltage spikes).

ENSURE CORRECT VOLTAGE IS USED!! Please do not fry your LEDs.

## Resources

- [Raspberry Pi Pico datasheet](https://pip-assets.raspberrypi.com/categories/610-raspberry-pi-pico/documents/RP-008307-DS-2-pico-datasheet.pdf)
- [WS2812b datasheet](https://www.ledyilighting.com/wp-content/uploads/2025/02/WS2812B-datasheet.pdf)
- [Raspberry Pi Pico SDK documentation](https://www.raspberrypi.com/documentation/pico-sdk/)
