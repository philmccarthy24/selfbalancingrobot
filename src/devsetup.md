WiringPi 2.5 comes with Raspbian on Pi4.
As per http://wiringpi.com/reference/i2c-library/ we need to install i2c drivers into the kernel:

```
gpio load i2c
```
Oh - had to use sudo raspi-config to enable the i2c module via interfaces menu, then reboot.
