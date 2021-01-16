#include <iostream>
#include <memory>
#include <unistd.h>
#include "MPU6050.h"

using namespace std;
//using namespace sbr;

/*
void readFifoBuffer_() {
  // Clear the buffer so as we can get fresh values
  // The sensor is running a lot faster than our sample period
  mpu.resetFIFO();
  
  // get current FIFO count
  fifoCount = mpu.getFIFOCount();
  
  // wait for correct available data length, should be a VERY short wait
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

  // read a packet from FIFO
  mpu.getFIFOBytes(fifoBuffer, packetSize);
}
*/

int main()
{
    cout << "hello world" << endl;

    MPU6050 mpu;
    mpu.initialize();
    //mpu.dmpInitialize();

    /*mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    float pitch = ypr[1] * 180/M_PI;
    */


    /*
    try {

    
        std::unique_ptr<ISpacialSensor> pSpacialSensor = make_unique<MPU6050>();

        for (int i = 0; i < 100; i++) {
            auto currOrientation = pSpacialSensor->ReadOrientation();
            cout << "X rotation " << currOrientation.m_dbPitch_deg << " degrees, Y rotation " << currOrientation.m_dbRoll_deg << "degrees" << endl;
            usleep(500 * 1000);
        }
    }
    catch (const std::exception& ex) {
        cout << ex.what() << endl;
    }
    */
}

//https://code.visualstudio.com/docs/languages/cpp

//http://wiringpi.com/reference/i2c-library/

//https://sudonull.com/post/97828-Ruby-and-C-Part-4-Friends-of-the-accelerometer-gyroscope-and-range-finder-with-Raphaeljs


//https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing ??