#include "MPU6050Accel.h"
#include "II2CDevice.h"
#include "AHRS.h"
#include <cmath>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace sbrcontroller::sensors;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            const int MPU6050Accel::ACCEL_FS_COUNTS[] = {
                16384, // range of accelerometer is +-2g
                8192, // range of accelerometer is +-4g
                4096, // range of accelerometer is +-8g
                2048  // range of accelerometer is +-16g
            };

            MPU6050Accel::MPU6050Accel(std::shared_ptr<coms::II2CDevice> pI2CDevice) :
                m_pMPU6050(pI2CDevice) 
            {
                // disable sleep mode by setting PWR_MGMT_1 register to 0
                m_pMPU6050->WriteReg8(PWR_MGMT_1, 0x00);

                // ACCEL_CONFIG register. AFS_SEL is bits 3 and 4
                int ACCEL_FS = (m_pMPU6050->ReadReg8(ACCEL_CONFIG) >> 3) & 0x3;
                m_nCountsPerG = ACCEL_FS_COUNTS[ACCEL_FS];

                //... there's a way to do a calibration as well
                // for both accel and gyro. Should we support this?
            }

            MPU6050Accel::~MPU6050Accel()
            {
            }

            SensorInfo MPU6050Accel::GetDeviceInfo()
            {
                SensorInfo si = {};
                si.sensorType = ESensorType::Accelerometer;
                si.dataType = ESensorDataType::TripleAxis_Acceleration_G;
                si.requiredDataLength = sizeof(TripleAxisData);
                si.identifier = "MPU6050 Accelerometer device"; // might be able to use i2c to query hardware
                return si;
            }
            
            int MPU6050Accel::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                if (length < sizeof(TripleAxisData))
                    return 0;

                // read raw counts from accelerometer and gyroscope. page 29 of [RegisterMap]
                short accXRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(ACCEL_XOUT_H));
                short accYRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(ACCEL_YOUT_H));
                short accZRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(ACCEL_ZOUT_H));
                // convert to units
                auto pData = reinterpret_cast<TripleAxisData*>(buffer);
                pData->x = static_cast<float>(accXRawCounts) / static_cast<float>(m_nCountsPerG);
                pData->y = static_cast<float>(accYRawCounts) / static_cast<float>(m_nCountsPerG);
                pData->z = static_cast<float>(accZRawCounts) / static_cast<float>(m_nCountsPerG);
                
                return sizeof(TripleAxisData);
            }

        }
    }
}