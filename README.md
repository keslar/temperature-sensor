# Temperature Sensor

This is a temperature and humidity sensor based upon the [Adafruit  Feather HUZZAH with ESP8266](https://www.adafruit.com/product/2821) connected to a [Adafruit Si7021 Temperature & Humidity Sensor](https://www.adafruit.com/product/3251).

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system. 

### Prerequisites

To build the remote sensor:

-  [Adafruit  Feather HUZZAH with ESP8266](https://www.adafruit.com/product/2821)
- [Adafruit Si7021 Temperature & Humidity Sensor](https://www.adafruit.com/product/3251).
- A li-poly battery like: [Lithium Ion Polymer Battery - 3.7v 150mAh](https://www.adafruit.com/product/1317)

For the Arduino development you will need to install:

- [CP2104 USB drivers](https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx) on your development workstation
- [ESP8266 Board Package](http://arduino.esp8266.com/stable/package_esp8266com_index.json) into the Arduino IDE ([howto](https://learn.adafruit.com/adafruit-feather-huzzah-esp))
- [Adafruit Si7021 Library](https://github.com/adafruit/Adafruit_Si7021/archive/master.zip) ([howto](https://learn.adafruit.com/adafruit-si7021-temperature-plus-humidity-sensor?view=all#download-adafruit-si7021))
- [TimedAction](http://playground.arduino.cc/Code/TimedAction) library

For the webserver you will need:

​	I don't know yet. I haven't written the server side yet.

### Installing

#### Build Remote Sensor

Attach the Temperature and Humidity sensor to the Arduino:

1.  Connect **Vin** to the power supply (3V) on the Feather. 
2. Connect **GND** to common power/data ground (GND) on the Feather.
3. Connect the **SCL** pin to the I2C clock **SCL** pin on the Feather.
4. Connect the **SDA** pin to the I2C data **SDA** pin on the Feather.
5. Attach the battery to the battery connector on the Feather.
6. Mount the battery, feather, and sensor into the case.

#### Setup Arduino IDE

1. Load USB Driver
2. Install Arduino IDE
3. Add ESP8266 board package to Arduino IDE
4. Add Si7021 Library

#### Setup Web Development

tbd

## Running the tests

Add additional notes about how to deploy this on a live system 

## Deployment

Add additional notes about how to deploy this on a live system 

## Contributing

Please read CONTRIBUTING.md for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](https://semver.org/) for versioning. For the versions available, see [the tags on this repository](https://github.com/keslar/temperature-sensor/tags).

## Authors

- **Chris Keslar** - *Initial work*

See also the list of [contributors](https://github.com/keslar/temperature-sensor/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the LICENSE.md file for details.

## Acknoledgements