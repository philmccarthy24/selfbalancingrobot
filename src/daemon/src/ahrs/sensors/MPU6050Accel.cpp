#include "MPU6050Accel.h"
#include <cmath>
#include <stdexcept>
#include <chrono>

using namespace std;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            const int MPU6050::ACCEL_FS_COUNTS[] = {
                16384, // range of accelerometer is +-2g
                8192, // range of accelerometer is +-4g
                4096, // range of accelerometer is +-8g
                2048  // range of accelerometer is +-16g
            };

            MPU6050::MPU6050(std::shared_ptr<coms::II2CDevice> pI2CDevice) :
                m_pMPU6050(pI2CDevice) // 50Hz (20ms pauses) is prob best we can hope to sample at for non-RTOS
            {
                m_fdMPU6050 = wiringPiI2CSetup(MPU6050_addr);
                if (m_fdMPU6050 == -1) {
                    throw runtime_error("Could not setup MPU6050");
                }
                // disable sleep mode by setting PWR_MGMT_1 register to 0
                wiringPiI2CWriteReg8(m_fdMPU6050, PWR_MGMT_1, 0x00);

                // ACCEL_CONFIG register. AFS_SEL is bits 3 and 4
                int ACCEL_FS = (wiringPiI2CReadReg8(m_fdMPU6050, ACCEL_CONFIG) >> 3) & 0x3;
                m_nCountsPerG = ACCEL_FS_COUNTS[ACCEL_FS];
                
                // GYRO_CONFIG register. FS_SEL is bits 3 and 4
                int FS_SEL = (wiringPiI2CReadReg8(m_fdMPU6050, GYRO_CONFIG) >> 3) & 0x3;
                m_fDegreesPerSec = GYRO_FS_COUNTS[FS_SEL];

                //... there's a way to do a calibration as well
                // for both accel and gyro. Should we support this?

                
            }

            MPU6050Accel::~MPU6050Accel()
            {
            }

            unsigned short MPU6050Accel::ReadHighLowI2CRegisters(int highRegister)
            {
                int highByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister);
                int lowByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister + 1);
                return (highByte << 8) | lowByte;
            }



            SensorData MPU6050Accel::GetData() 
            {
                // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
                short accXRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_XOUT_H));
                short accYRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_YOUT_H));
                short accZRawCounts = static_cast<short>(ReadHighLowI2CRegisters(ACCEL_ZOUT_H));
                // convert to units
                float accX = static_cast<float>(accXRawCounts) / static_cast<float>(m_nCountsPerG);
                float accY = static_cast<float>(accYRawCounts) / static_cast<float>(m_nCountsPerG);
                float accZ = static_cast<float>(accZRawCounts) / static_cast<float>(m_nCountsPerG);
            }


}
}