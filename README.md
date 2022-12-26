# MAP Sensor Graph

This is a GUI based on QCustomPlot to plot the pressure measurements of the intake manifold pressure (MAP) sensor of a diesel engine.
In order to read the data, an OBD-Connector with an ELM327 (based on a PIC18 Microcontroller) has to be used.

## Helpful Links 
OBDII-PID's: [Wikipedia](https://en.wikipedia.org/wiki/OBD-II_PIDs#Service_02_-_Show_freeze_frame_data)

The original OBD-ELM327-Communication Library: [gasparian](https://github.com/gasparian/obd_elm327_vehicle_speed)

## Additional Information
The whole code was written on christmast eve and i had no experience with qt and OBD until then so it's kind of messy. But it's quite helpful to detect leakages or errors in the intake system of your engine.
In my case i had a faultcode P0107 and needed to check if the sensor works properly. So I disconnected the between the turbo charger and the intake air filter
and connected it to a compressor. Then I pressurized the whole stuff with pressures between one and two bars. Comparing the MAP-Sensor Reading with the manometer of the compressor
showed that the sensor was working properly so there hat to be a leakage somwhere. Using some soap finally helped to find the leakage.

If there is some time I'm going to clean up the code and add additional functions/sensors.
