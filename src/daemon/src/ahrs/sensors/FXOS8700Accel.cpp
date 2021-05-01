#include "FXOS8700Accel.h"
#include "II2CDevice.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include "sbrcontroller.h"
#include <cmath>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace sbrcontroller::sensors;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            const std::map<FXOS8700Accel::EAccelRange, float> FXOS8700Accel::m_RangeSensitivities = {
                                    
                {ACCEL_RANGE_2G, 0.000244F}, // mg per LSB at +/- 2g sensitivity (1 LSB = 0.000244mg)
                {ACCEL_RANGE_4G, 0.000488F}, //mg per LSB at +/- 4g sensitivity (1 LSB = 0.000488mg)
                {ACCEL_RANGE_8G, 0.000976F} // mg per LSB at +/- 8g sensitivity (1 LSB = 0.000976mg)
            };

            FXOS8700Accel::FXOS8700Accel(std::shared_ptr<coms::II2CDevice> pI2CDevice, const sbrcontroller::sensors::TripleAxisData& calibration, std::shared_ptr<spdlog::logger> pLogger, EAccelRange accelRange) :
                m_pFXOS8700(pI2CDevice),
                m_calibrationOffsets {calibration},
                m_pLogger(pLogger)
            {
                // Make sure we have the correct chip ID since this checks
                // for correct address and that the IC is properly connected
                uint8_t id = m_pFXOS8700->ReadReg8(FXOS8700_REGISTER_WHO_AM_I);
                if (id != FXOS8700_ID) {
                    throw errorhandling::InvalidDeviceException(fmt::format("Expected id {:0x} but received id {:0x}", (int)FXOS8700_ID, id));
                }

                m_pLogger->debug("Using Zero Rate Offset {}, {}, {}", m_calibrationOffsets.x, m_calibrationOffsets.y, m_calibrationOffsets.z);

                // Set to standby mode (required to make changes to this register)
                m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_CTRL_REG1, 0);

                const auto& si = m_RangeSensitivities.find(accelRange);
                if (si == m_RangeSensitivities.end())
                    throw errorhandling::BadLogicException(fmt::format("Sensitivity for range {} not specified", accelRange));
                m_unitScale = si->second;

                // Configure the accelerometer
                switch (accelRange) {
                case (ACCEL_RANGE_2G):
                    m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_XYZ_DATA_CFG, 0x00);
                    break;
                case (ACCEL_RANGE_4G):
                    m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_XYZ_DATA_CFG, 0x01);
                    break;
                case (ACCEL_RANGE_8G):
                    m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_XYZ_DATA_CFG, 0x02);
                    break;
                }
                // High resolution
                m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_CTRL_REG2, 0x02);
                // Active, Normal Mode, Low Noise, 100Hz in Hybrid Mode
                m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_CTRL_REG1, 0x15);
            }

            FXOS8700Accel::~FXOS8700Accel()
            {
            }

            SensorInfo FXOS8700Accel::GetDeviceInfo()
            {
                SensorInfo si = {};
                si.sensorType = ESensorType::Accelerometer;
                si.dataType = ESensorDataType::TripleAxis_Acceleration_G;
                si.requiredDataLength = sizeof(TripleAxisData);
                si.identifier = "FXOS8700 Accelerometer device";
                return si;
            }

            void FXOS8700Accel::ClearCalibration()
            {
                m_calibrationOffsets = {};
            }
            
            int FXOS8700Accel::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                if (length < sizeof(TripleAxisData))
                    return 0;

                // TODO: Would be really nice to work out how to do an I2C block read of 6 bytes! Rather than 
                // 3 2-byte word reads.
                // read raw counts from accelerometer, shifting values to create properly formed integers
                // Note, accel data is 14-bit and left-aligned, so we shift two bit right
                // Also note the order of shift is important - shift a signed short or will get bad values
                short accXRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_OUT_X_MSB)) >> 2;
                short accYRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_OUT_Y_MSB)) >> 2;
                short accZRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_OUT_Z_MSB)) >> 2;

                m_pLogger->trace("14 bit shifted counts (decimal): x={}, y={}, z={}", accXRawCounts, accYRawCounts, accZRawCounts);

                // Compensate values depending on the resolution - counts to units
                // convert to G units
                auto pData = reinterpret_cast<TripleAxisData*>(buffer);
                pData->x = static_cast<float>(accXRawCounts) * m_unitScale;
                pData->y = static_cast<float>(accYRawCounts) * m_unitScale;
                pData->z = static_cast<float>(accZRawCounts) * m_unitScale;

                // apply calibration
                pData->x -= m_calibrationOffsets.x;
                pData->y -= m_calibrationOffsets.y;
                pData->z -= m_calibrationOffsets.z;

                m_pLogger->debug("x={}, y={}, z={}", pData->x, pData->y, pData->z);
                
                return sizeof(TripleAxisData);
            }

        }
    }
}