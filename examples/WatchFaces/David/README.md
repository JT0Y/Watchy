# David's watch faces


## Watch faces
This contains some watch-faces for the Watchy from SQFMI:
 - Step counter
 - Jarvis
 - Back to the future

*Note: To change between different watch faces adapt the Watchy.ino file. Currently, only one watch face is loaded. In future I will implement it such that you can switch between my watch-faces during runtime. But currently this is not possible...*


<img src="doc/bttf.jpg" alt="Back to the future" />



## Buttons and other functionality
Some basic functionality is provided for all my watchfaces via the Watchy_Base class.
Namely, the buttons are used and an MQTT publisher is implemented such that 
you can send actions to other IOT devices using you watchy. Therefore, also 
the PubSubClient library must be installed within your Arduino environment.


## Config
Please create a config.h file containing the following definitions:
```
#define WIFI_SSID      "ENTER_HERE"
#define WIFI_PASS      "ENTER_HERE"

#define MQTT_BROKER    "ENTER_HERE"
#define MQTT_TOPIC     "ENTER_HERE"
#define MQTT_PAYLOAD   "ENTER_HERE"
```
This ensures that you never make this information available through git.

# Acknowledgement
Thanks a lot to SQFMI (https://github.com/sqfmi/Watchy) for this great project!