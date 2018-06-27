# Bidon Dispenser 4.0
Smart Solutions Semester - Join the Pipe - Year 17/18 - Semester 02



# Authors
All the software is written  by Robin C. Pel   with student number 421 431.
All the hardware is designed by Glen te Hofsté with student number 419 720.



# Note to self
Use "git merge --allow-unrelated-histories [ORPHAN-BRANCH]" to merge orphan branches.



# While Loops
All the while loops in the Microcontroller's program:
- "./Communication/OneWire.cpp" line 13: Wait for a pin to be high
- "./Communication/USART.cpp" line 34: Wait for new data on the serial port
- "./Communication/USART.cpp" line 42: Wait for new data on the serial port for 2 seconds
- "./Hardware/SystemClock.cpp" line 13: Wait for the clock source to be ready
- "./Master/RaspberryPi.cpp" line 36: Wait until the pre-amble is found
- "./Sensors/DistanceSensor.cpp" line 93: Wait for a pin to be high
- "./Sensors/DistanceSensor.cpp" line 93: Wait for a pin to be low
- "./Sensors/TemperatureSensor.cpp" line 33: Wait for a pin to be high
- "./main.cpp" line 197, 280, 323, 362 and 384: Infinite loops



# Important
Not all code on the "microcontroller/software/develop" branch works with all the microcontroller hardware:
	- All the commits BEFORE the tag "MCU_Festival" only works on the controller PCB v1.1
	- All the commits AFTER  the tag "MCU_Festival" only works on the controller PCB v2.0

Another thing that's definitely worth mentioning is that on the microcontroller PCB v1.1 the ADC was used to read a 1 or 0.
This was done because for some weird reason it was not possible to read one of the digital pins, 
the microcontroller probably was broken or something but the decision was made to work around it for the time being.

After the Smart Solutions Festival one of the distance sensors broke, this sensor was distance sensor number five.

/** Distance Sensor Layout Layout (Top view, from the front):
 Column:        00  01  02  03  04  05  06  07
 Dist Sensor:  [08][01][10][03][12][05][14][07]
 Dist Sensor:  [00][09][02][11][04][13][06][15]
 */



# Disclaimer
This was my first experience with C# and the Visual Studio IDE, there are probably a lot of things which could be optimized. 
This git repo was also misused a little bit, apologies. 
There is no easter egg. 



# Regarding the UI Assets
All UI Assets were made using Paint dot net, you can get it here https://www.dotpdn.com/downloads/pdn.html



![](TeamPhoto/TeamPhoto.png "The team.")