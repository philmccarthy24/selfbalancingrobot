#include <iostream>
#include <memory>
#include <unistd.h>
#include "MPU6050.h"

using namespace std;
using namespace sbr;

int main()
{
    try {

    
        std::unique_ptr<ISpacialSensor> pSpacialSensor = make_unique<MPU6050>();

        for (int i = 0; i < 100; i++) {
            auto currOrientation = pSpacialSensor->ReadOrientation();
            cout << "X rotation " << currOrientation.first << " degrees, Y rotation " << currOrientation.second << "degrees" << endl;
            usleep(500 * 1000);
        }
    }
    catch (const std::exception& ex) {
        cout << ex.what() << endl;
    }
}

//https://code.visualstudio.com/docs/languages/cpp

//http://wiringpi.com/reference/i2c-library/

//https://sudonull.com/post/97828-Ruby-and-C-Part-4-Friends-of-the-accelerometer-gyroscope-and-range-finder-with-Raphaeljs


//https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing ??