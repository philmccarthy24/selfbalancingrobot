# Live Raspbian dev environment setup

## I2C setup

WiringPi 2.5 comes with Raspbian Buster on Pi4.
As per http://wiringpi.com/reference/i2c-library/ the i2c drivers need to be installed into the kernel:

```
sudo raspi-config
```
Then enable the i2c module via interfaces menu, and reboot.

## VSCode setup



# Cross compilation on non-PI platform (MacOS)

TODO. Looks like a pain in the butt.