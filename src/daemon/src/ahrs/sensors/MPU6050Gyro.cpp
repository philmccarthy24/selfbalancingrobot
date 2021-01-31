#include "MPU6050Gyro.h"
#include "II2CDevice.h"
#include "AHRS.h"
#include <cmath>
#include <stdexcept>

using namespace std;
using namespace sbrcontroller::sensors;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            // these are held as radian values
            const double MPU6050Gyro::GYRO_FS_COUNTS[] = {
                (131 * M_PI) / 180, // range of gyroscope is +-250deg/s
                (65.5 * M_PI) / 180, // range of gyroscope is +-500deg/s
                (32.8 * M_PI) / 180, // range of gyroscope is +-1000deg/s
                (16.4 * M_PI) / 180  // range of gyroscope is +-2000deg/s
            };

            MPU6050Gyro::MPU6050Gyro(std::shared_ptr<coms::II2CDevice> pI2CDevice) :
                m_pMPU6050(pI2CDevice) 
            {
                // disable sleep mode by setting PWR_MGMT_1 register to 0
                m_pMPU6050->WriteReg8(PWR_MGMT_1, 0x00);

                // GYRO_CONFIG register. FS_SEL is bits 3 and 4
                int FS_SEL = (m_pMPU6050->ReadReg8(GYRO_CONFIG) >> 3) & 0x3;
                m_fRadiansPerSec = GYRO_FS_COUNTS[FS_SEL];

                //... there's a way to do a calibration as well
                // for both accel and gyro. Should we support this?
            }

            MPU6050Gyro::~MPU6050Gyro()
            {
            }

            SensorInfo MPU6050Gyro::GetDeviceInfo()
            {
                SensorInfo si = {};
                si.sensorType = ESensorType::Gyroscope;
                si.dataType = ESensorDataType::TripleAxis_RotationalVelocity_RadiansPerSec;
                si.requiredDataLength = sizeof(TripleAxisData);
                si.identifier = "MPU6050 Gyroscope device"; // might be able to use i2c to query hardware
                return si;
            }
            
            int MPU6050Gyro::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                if (length < sizeof(TripleAxisData))
                    return 0;

                // read raw counts from gyroscope. page 29 of [RegisterMap]
                short gyXRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(GYRO_XOUT_H));
                short gyYRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(GYRO_YOUT_H));
                short gyZRawCounts = static_cast<short>(m_pMPU6050->ReadReg16(GYRO_ZOUT_H));
                // convert to units
                auto pData = reinterpret_cast<TripleAxisData*>(buffer);
                pData->x = static_cast<float>(gyXRawCounts) / static_cast<float>(m_fRadiansPerSec);
                pData->y = static_cast<float>(gyYRawCounts) / static_cast<float>(m_fRadiansPerSec);
                pData->z = static_cast<float>(gyZRawCounts) / static_cast<float>(m_fRadiansPerSec);
                
                return sizeof(TripleAxisData);
            }

        }
    }
}