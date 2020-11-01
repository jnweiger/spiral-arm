# Spiral Arm
An articulated arm made out of plywood. Its movement reminds me of an unfolding fern leaf, or a (somewhat stiff) octopus arm.

This wonderful design was [published by John Edmark](http://www.johnedmark.com/spirals1/2016/4/29/roll-up-spiral).
He is also known for his [helicone](http://www.johnedmark.com/rotating1/2016/4/29/helicone-an-interactive-kinetic-sculpture).

The lasercut parts are designed with inkscape. The sides of the arm are drawn in their rolled up position, they almost completely fill the plane.
You can still see how the grains of wood continue, when the arm fully rolled up.
I am adding different amounts of spacers between the two sides, to that the arm becomes a more natural 3-dimensional object.
The sides are cut out of 4mm birch plywood using arm-8er-shaped_laser3mm.svg (arm-8er-shaped_laser4mm.svg has the same for 3mm sides).
Either svg file also includes the needed spacers. For the spacers you need 3, 4 and 5 mm plywood, so that they can be stacked for all the 
different widths needed. See spacers.txt for the exact stacking. The shafts for mounting the segments are cut from beechwood rods with are 
6mm, 4mm, 3mm, and 2mm diameter. The 2mm ones are simply toothpics. I love toothpicks. You'll need lots of them, the often also serve as alignment pins.
My design includes 26 segments, with 8 segments per revolution.

You can hold the arm in your hands and pull the string to play with it. But as it is rather large and heavy, I also made a mount and an electric drive for it.

The mount is made out of 8mm plywood. Gluing the tabs firmly into the base plate is important. As the arm gives a strong pull there, when fully extended.
The base should be secured to the table with a clamp or several kg of counterweight to prevent it from flipping over. 
There are several holes in the base. You can use them to mount the base on a tripod (if you dare), or connect multiple bases side by side fanning out the arms in
in 15°, 30° or 45° angle (just in case you want to complete an octopus eventually).

The electric drive is based on an inexpensive worm gear motor found on ebay. It runs from 12v and takes less than 1A. 
It is built into a gearbox with lasercut gears from 3mm POM and a 3D-printed spool for winding up the string. The 3D-print design was made with FreeCAD.
All the STL-files in the drive folder were exported from the drum.FCstd drawing.

The motor is controlled by an Atmel ATTINY 2313 microcontroller connected to an L298N motor bridge. 
the logic is very simple. The winds up the string, until the arm is fully curled up and hits a push-button in the mount. Then the motor rotation is reversed, so that it unrolls again. There is no reversal needed for the unrolled position, as the spool simply winds up the string again after it was fully unrolled. 

You can also use an arudino-nano or similar instead of the bare metal 2313 -- programming a nano or uno is easier than. But for some reason I prefered to be minimalistic with the electronics. (Well, the 128 bytes RAM are almost full.)

If you can come up with a mechanical reversal switch that reliably works with this arm, please let me know. I failed to come up with something sufficiently robust, and went for the electronic solution instead. 

