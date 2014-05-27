Sleeplock
=========

Sleeplock is a system which automatically locks a door when his user lies down on bed. As a result, the user does not need to worry about whether or not he has locked the door - especially useful for single older people.

I am currently developing a prototype with Arduino for testing and proving this system could really work in real life. Components for my prototype are:
- Arduino Mega 2560
- Step motor, 28BYJ-48
- Force sensitive resistor (2)
- LED (3)
- Button

====

#### Testing & Debugging
Setting the following boolean to true will start the test mode, which means the serial monitor will output the necessary information for testing and debugging.
```js
boolean testMode = false;
```

Setting the following boolean to true will start the sensor debugging, which means the serial monitor will keep on outputting the measurements of the FSRs.
```js
boolean sensorDebugging = false;
```