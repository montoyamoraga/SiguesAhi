# are-you-still-here

## About

"Are you still here?" is is a project by Aarón Montoya-Moraga.

## The artwork

This is a collection of three different sculptures.

## Hardware

This project is built on a Arduino Nano 33 IOT.

## Software

The code included is for flashing the Arduino.

create a file called internetLogin.h via the Arduino IDE.

Add this code

```h
#define SECRET_SSID     "your_ssid"
#define SECRET_PASS     "your_pass"
```


## Documentation

## Do it yourself

You can change these variables:

* answerType:
  * answerType = 0, beep
  * answerType = 1, print
  * answerType = 2, servo
* interval:
  * interval = 0, 1 minute
  * interval = 1, 1 hour
  * interval = 2, 1 day

## Inspiration

This project was inspired by [Luke Dubois](http://lukedubois.com/)' [Take a Bullet for This City](http://sites.bxmc.poly.edu/~lukedubois/projects/index.html?id=gun) (2014)

## License

MIT
