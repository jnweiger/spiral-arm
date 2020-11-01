Program a blink sketch into an attiny2313
=========================================

I have an 
* old 16pin DIL programming clip
* an USBtiny ISP
* adruino IDE 1.8.13
* avrdude installed (for its /lib/udev/rules.d/60-avrdude.rules)

The Tiny2313 is a 20pin chip.
To program that one, we break lose one leg of the 16in clip and extend it by two pins.
This is the ground pin.

To verify program success, we solder an LED with a 330ohm resistor between GND and the next 
available pin on the clip, Pin 7.

In arduino IDE load the Examples -> 01.Basics -> Blink.
Instead of BUILTIN_LED use PIN_PD3. (Yes we can use Atmel Pin nam PD3, no need for consult a fancy arduino specific numbering pattern!)

Prefernces -> Additional Boardmanager URLs ->
 add to the list: http://drazzy.com/package_drazzy.com_index.json

Tools -> Board -> Board Manager
  -> locate ATTinyCore by Spence Konde -> install.

Tools -> Board: 2313 (no bootloader)
Tools -> Chip: 2313 (no bootloader)
Tools -> Clock: 1Mhz internal
Tools -> Port: /dev/ttyACM0
Tools -> Programmer: USBtinyISP SLOW		- the fast one does not work for me.

disconnect, reconnect the USBtinyISP dongle. The green led comes on.

Save, Upload, The red led on the USBtinyISP is on while uploading.

There will be a compiler warning:
  Global variables use 105 bytes (82%) of dynamic memory, leaving 23 bytes for local variables. Maximum is 128 bytes.
  Low memory available, stability problems may occur.
That is fine, we currently use 2 bytes of local variables. Need a 4313 if we run into trouble here. It has 256 bytes ram. tha tis twice as much!

When upload is done, the LED on the clip starts blinking.
SUCCESS!
