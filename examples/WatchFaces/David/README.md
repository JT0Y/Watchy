# David's watch faces

This project contains different watch-faces for the watchy from SQFMI and 
it also integrates the watchy into a smart-home via MQTT.

Some basic functionality is provided for all my watchfaces via the Watchy_Base 
class. For example the *sleep-mode* starts at 01:00 and statically displays 
an image, until a button is pressed to increase battery life further (~ 3 days).

Another feature that is provided is, that the buttons are used:
 - upper left to set an alarm
 - upper right to read weather data from an mqtt broker
 - lower right to trigger something via MQTT (e.g. open a door)

Note: The PubSubClient library must be installed within your Arduino environment.


## Watch faces
The following watch-faces are currently implemented:
- Step counter
- Jarvis
- Back to the future (BTTF)
- Analog
- BCD
- LCARS


*Note: To change between different watch faces adapt the Watchy.ino file. Currently, only one watch face is loaded. In future I will implement it such that you can switch between my watch-faces during runtime. But currently this is not possible...*


## Smart-home integration
Please create a config.h file with the following definitions:
```
#define WIFI_SSID      "ENTER_HERE"
#define WIFI_PASS      "ENTER_HERE"

#define MQTT_BROKER    "ENTER_HERE"
#define MQTT_TOPIC     "ENTER_HERE"
#define MQTT_PAYLOAD   "ENTER_HERE"
```
This should ensure that you never make this information available through git.


# Acknowledgement
- Thanks a lot to SQFMI (https://github.com/sqfmi/Watchy) for this great project!
- https://github.com/BenjaminGabel/AnalogWatchFace