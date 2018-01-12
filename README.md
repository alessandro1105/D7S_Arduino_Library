# D7S Arduino Library

This library improve the use of the D7S sensor developed by Omron. It is compatibile with Arduino avr based boards, esp8266 boards and all flavour of Fishino.

## Getting Started

Just download the lastest realease and place it your Arduino IDE library folder. See [https://www.arduino.cc/en/Guide/Libraries](https://www.arduino.cc/en/Guide/Libraries)

### Installing

The D7S sensor must be connected to the I2C bus.

On most Arduino boards, SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5. On the Arduino Mega, SDA is digital pin 20 and SCL is 21 (See [https://www.arduino.cc/en/Guide/Libraries](https://www.arduino.cc/en/Reference/Wire)).

To use interrupt events provided by the D7S sensor you need to attach INT1 and INT2 pins to the interrupt pins of the boards you are using (See [https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) or [http://esp8266.github.io/Arduino/versions/2.1.0-rc2/doc/reference.html](http://esp8266.github.io/Arduino/versions/2.1.0-rc2/doc/reference.html)). The interrupt pins of Fishino32 are 3, 5, 6 and 9.

## Authors

* **Alessandro Pasqualini** - [alessandro1105](https://github.com/alessandro1105)

## Contributors

This project has been developed with the contribution of [Futura Elettronica](http://www.futurashop.it), [Elettronica In](http://www.elettronicain.it), [Open Electronics](https://www.open-electronics.org).

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details
