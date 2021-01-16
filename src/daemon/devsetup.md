# Live Raspbian dev environment setup

## I2C setup

Couple of choices interfacing with I2C: WiringPi 2.5 comes with Raspbian Buster on Pi4.
(http://wiringpi.com/reference/i2c-library/)
The Jeff Rowberg MPU6050 lib that makes use of the onboard motion processor / FIFO and returns
Quarternions that can be converted into roll/pitch/yaw, uses the BCM2835 lib here:
http://www.airspayce.com/mikem/bcm2835/index.html
This is better as we don't then need to do further filtering (eg complementary / kalman from seperate raw sensors)
bcm2835 is installed using
```
tar zxvf bcm2835-1.xx.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
```

The i2c drivers need to be enabled to the kernel:

```
sudo raspi-config
```
Then enable the i2c module via interfaces menu, and reboot.

## VSCode setup



# Cross compilation on non-PI platform (MacOS)

TODO. Looks like a pain in the butt.