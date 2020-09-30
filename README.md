# are-you-still-here

## About

"Are you still here?" is is a project by Aarón Montoya-Moraga, started in September 2020.

This project is a digital instrument to check if institutions still exist, and signal when they don't anymore.

This instrument connects to Wikipedia via internet, and checks if the corresponding article is in present or past tense, to determine if the institution exists or not. When the institution ceases to exist, the instrument signals with either beeping, motion, or text.

Instructions are included in this repository to build and program your own.

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
