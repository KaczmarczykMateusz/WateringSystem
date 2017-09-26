Watering System project is made to control entire green plants watering system basing on data from sensors:
- temperature (DS18B20),
- humidity,
- moisture,
- pressure, 
- brightness.
It uses data from;
- water flow sensor
in order to dose correct amount of water.

System uses: 
-time settings, 
- moisture of the soil,
- brightness,
- temperature(eventually),
to determine when to start water pump. All of the factors can be used combined or rether one by one, 
it is not decided yet.

IDE - WinAVR.
Hardware:
atmega8,
atmega32.

25.08.2017 - frame of the project already exists, repository is created in order to develop the project, first by adding 
semi-ready code, then developing it as whole.

06.09.2017
Basic development and debug version without communication 
-note tat constant 15 bit ADC (oversampling) is disabling interruptions for too long (debug only)

15.09.2017
After implementing UART there were huge issues with reseting/ hanging up of uC, also receiving and sending random data was problem.
Only disabling Real Time Clock was helping.
BUT all issues are solved now by adding two 100nF capacitors between both (Rx & Tx) pins of uC and GND