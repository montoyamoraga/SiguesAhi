# are-you-still-here

## About

"Are you still here?" is is a project by Aarón Montoya-Moraga, started in September 2020.

This project is a digital instrument to check if institutions still exist, and alert when they don't anymore.

The instrument periodically pings Wikipedia, and checks if the article is written in present or past tense. When it changes to past tense, the institution is no more, and the instrument emits beeping, text, or motion.

In this repository you can find documentation on how it works and instructions to build your own version.

## The artwork

This is a collection of three different sculptures.

## Hardware

This project is built on a Arduino Nano 33 IOT.

## Software

The code included is for flashing the Arduino.

create a file called internetLogin.h via the Arduino IDE.

Add this code

```h
#define NETWORK_NAME     "yourNetworkName"
#define NETWORK_PASS     "yourNetworkPass"
```


## Documentation

## Do it yourself

### Bill of materials


Core materials:
* Arduino Nano 33 IoT, $19.00 USD, available at [https://store.arduino.cc/usa/nano-33-iot]()

Optional materials:
* Mini thermal receipt printer starter pack, $61.95 USD, available at [https://www.adafruit.com/product/600]()
* Piezo buzzer, $1.50 USD, available at [https://www.adafruit.com/product/160]()

### Programming

* Download the Arduino IDE
* Add the following tools and libraries for the Arduino Nano 33 IoT


### Installing


You can change these variables:

* answerType:
  * answerType = 0, beep
  * answerType = 1, print
  * answerType = 2, servo
* interval:
  * interval = 0, 1 minute
  * interval = 1, 1 hour
  * interval = 2, 1 day

## Acknowledgements

This project was inspired by these artworks:

* [Take a Bullet for This City](http://sites.bxmc.poly.edu/~lukedubois/projects/index.html?id=gun), [Luke Dubois](http://lukedubois.com/), 2014

This project could not have been possible without the Wikipedia and Arduino projects.

## License

MIT
