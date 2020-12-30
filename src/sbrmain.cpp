#include <iostream>
#include <wiringPiI2C.h>

// compile with 
// g++ -g sbrmain.cpp -lwiringPi -o sbrmain

using namespace std;

const int MPU6050_addr=0x68;

int ReadHighLowI2CRegisters(int fd, int highRegister)
{
    int highByte = wiringPiI2CReadReg8(fd, highRegister);
    int lowByte = wiringPiI2CReadReg8(fd, highRegister + 1);
    return (highByte << 8) | lowByte;
}

int main()
{
    cout << "Hello World" << endl;

    int fd_mpu6050 = wiringPiI2CSetup(MPU6050_addr);
    if (fd_mpu6050 != -1) {
        // disable sleep mode by setting PWR_MGMT_1 register to 0
        wiringPiI2CWriteReg8(fd_mpu6050, 0x6B, 0x00);

        // accelerometer sample rate - defined in reg 25
        // page 14 of the register map doc shows how to do
        // self-test of the gyroscope axes. Might be useful to do


        // ACCEL_CONFIG register. AFS_SEL is bits 3 and 4
        int ACCEL_FS = (wiringPiI2CReadReg8(fd_mpu6050, 0x1C) >> 3) & 0x3;
        // GYRO_CONFIG register. FS_SEL is bits 3 and 4
        int FS_SEL = (wiringPiI2CReadReg8(fd_mpu6050, 0x1B) >> 3) & 0x3;

        cout << "ACCEL_FS=" << ACCEL_FS << ", FS_SEL=" << FS_SEL << endl;
        
        // read some raw counts. page 29 of the register map doc
        int accX = ReadHighLowI2CRegisters(fd_mpu6050, 0x3B);
        int accY = ReadHighLowI2CRegisters(fd_mpu6050, 0x3D);
        int accZ = ReadHighLowI2CRegisters(fd_mpu6050, 0x3F);
        int gyX = ReadHighLowI2CRegisters(fd_mpu6050, 0x43);
        int gyY = ReadHighLowI2CRegisters(fd_mpu6050, 0x45);
        int gyZ = ReadHighLowI2CRegisters(fd_mpu6050, 0x47);

        cout << "AccelX=" << accX << ", AccelY=" << accY << ", AccelZ=" << accZ;
        cout << "GyroX=" << gyX << ", GyroY=" << gyY << ", GyroZ=" << gyZ << endl;

        // 16 bit signed accelerometer measurements are dependent on full
        // scale defined in ACCEL_FS - register 28
        // 0 is most sensitive, from -2 to 2Gs at 16384 counts per G
        //what is the default? should we read?

        // 16 bit signed gyroscope measurements are dependent on
        // scale defined in FS_SEL - register 27
        // 0 is most sensitive, from -250 to 250 degrees per second?
        // at 131 counts per degree per second (??)

        //... there's a way to do a calibration as well
        // for both accel and gyro
    }
    else 
    {
        cout << "Error setting up I2C connection to MPU6050." << endl;
    }

}

//https://code.visualstudio.com/docs/languages/cpp

//http://wiringpi.com/reference/i2c-library/

//https://sudonull.com/post/97828-Ruby-and-C-Part-4-Friends-of-the-accelerometer-gyroscope-and-range-finder-with-Raphaeljs


//https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing ??