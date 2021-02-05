# David's watch faces

This contains some watch-faces for the Watchy from SQFMI:
 - Step counter
 - Jarvis

Some basic functionality is provided for all my watchfaces via the Watchy_Base class.
Namely, the buttons are used and an MQTT publisher is implemented such that 
you can send actions to other IOT devices using you watchy. Therefore, also 
the PubSubClient library must be installed within your Arduino environment.