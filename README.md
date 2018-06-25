# Bidon Dispenser 4.0
Smart Solutions Semester - Join the Pipe - Year 17/18 - Semester 02



Note to self:
Use "git merge --allow-unrelated-histories [ORPHAN-BRANCH]" to merge orphan branches.



# Software related
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


