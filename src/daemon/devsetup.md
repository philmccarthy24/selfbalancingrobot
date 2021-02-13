# Live Raspbian dev environment setup

## I2C setup

The i2c drivers need to be installed into the kernel:
```
sudo raspi-config
```
enable the i2c module via interfaces menu, and reboot.

Then the i2c dev lib headers need to be installed:
```
sudo apt-get install libi2c-dev
```

## VSCode setup

Install extensions
- Microsoft C/C++ (>=1.1.3)
- CMake Tools (>=1.5.3)
- C++ TestMate (>=3.6.20)

```
sudo apt-get ninja cmake
```

# Cross compilation on non-PI platform (MacOS)

TODO. Looks like a pain in the butt.