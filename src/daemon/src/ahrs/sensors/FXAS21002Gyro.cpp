#include "FXAS21002Gyro.h"
#include "II2CDevice.h"
#include "sbrcontroller.h"
#include "AHRS.h"
#include "spdlog/spdlog.h"
#include <fmt/core.h>
#include <chrono>
#include <cmath>

using namespace std;
using namespace sbrcontroller::sensors;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            const std::map<FXAS2100Gyro::ERange, float> FXAS2100Gyro::m_RangeSensitivities = std::map<FXAS2100Gyro::ERange, float> {
                { GYRO_RANGE_250DPS, 0.0078125F },
                { GYRO_RANGE_500DPS, 0.015625F },
                { GYRO_RANGE_1000DPS, 0.03125F },
                { GYRO_RANGE_2000DPS, 0.0625F } 
            };

            FXAS2100Gyro::FXAS2100Gyro(std::shared_ptr<coms::II2CDevice> pI2CDevice, const sbrcontroller::sensors::TripleAxisData& calibration, std::shared_ptr<spdlog::logger> pLogger, ERange range) :
                m_pFXAS2100(pI2CDevice),
                m_calibrationOffsets {calibration},
                m_pLogger(pLogger)
            {
                // Make sure we have the correct chip ID since this checks
                // for correct address and that the IC is properly connected
                uint8_t id = m_pFXAS2100->ReadReg8(GYRO_REGISTER_WHO_AM_I);
                if (id != FXAS21002C_ID) {
                    throw errorhandling::InvalidDeviceException(fmt::format("Expected id {:0x} but received id {:0x}", (int)FXAS21002C_ID, id));
                }

                m_pLogger->debug("Using Zero Rate Offset {}, {}, {}", m_calibrationOffsets.x, m_calibrationOffsets.y, m_calibrationOffsets.z);
                
                uint8_t ctrlReg0 = 0x00;

                switch (range) {
                    case GYRO_RANGE_250DPS:
                        ctrlReg0 = 0x03;
                        break;
                    case GYRO_RANGE_500DPS:
                        ctrlReg0 = 0x02;
                        break;
                    case GYRO_RANGE_1000DPS:
                        ctrlReg0 = 0x01;
                        break;
                    case GYRO_RANGE_2000DPS:
                        ctrlReg0 = 0x00;
                        break;
                }

                const auto& si = m_RangeSensitivities.find(range);
                if (si == m_RangeSensitivities.end())
                    throw errorhandling::BadLogicException(fmt::format("Sensitivity for range {} not specified", range));
                m_unitScale = si->second;

                // Reset then switch to active mode with 100Hz output
                m_pFXAS2100->WriteReg8(GYRO_REGISTER_CTRL_REG1, 0x00);     // Standby
                m_pFXAS2100->WriteReg8(GYRO_REGISTER_CTRL_REG1, 0x20/*(1 << 6)*/); // Reset
                m_pFXAS2100->WriteReg8(GYRO_REGISTER_CTRL_REG0, ctrlReg0); // Set sensitivity
                m_pFXAS2100->WriteReg8(GYRO_REGISTER_CTRL_REG1, 0x0E);     // Active

                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 60 ms + 1/ODR
            }

            FXAS2100Gyro::~FXAS2100Gyro()
            {
            }

            SensorInfo FXAS2100Gyro::GetDeviceInfo()
            {
                SensorInfo si = {};
                si.sensorType = ESensorType::Gyroscope;
                si.dataType = ESensorDataType::TripleAxis_RotationalVelocity_RadiansPerSec;
                si.requiredDataLength = sizeof(TripleAxisData);
                si.identifier = "FXAS2100Gyro Gyroscope device";
                return si;
            }

            void FXAS2100Gyro::ClearCalibration()
            {
                m_calibrationOffsets = {0.0};
            }
            
            int FXAS2100Gyro::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                if (length < sizeof(TripleAxisData))
                    return 0;

                // read raw counts from gyroscope.
                short gyXRawCounts = static_cast<short>(m_pFXAS2100->ReadReg16(GYRO_REGISTER_OUT_X_MSB));
                short gyYRawCounts = static_cast<short>(m_pFXAS2100->ReadReg16(GYRO_REGISTER_OUT_Y_MSB));
                short gyZRawCounts = static_cast<short>(m_pFXAS2100->ReadReg16(GYRO_REGISTER_OUT_Z_MSB));

                // Compensate values depending on the resolution - counts to units
                float gyXDps = gyXRawCounts * m_unitScale;
                float gyYDps = gyYRawCounts * m_unitScale;
                float gyZDps = gyZRawCounts * m_unitScale;

                // convert to radians
                auto pData = reinterpret_cast<TripleAxisData*>(buffer);
                pData->x = (gyXDps * M_PI) / 180.0f;
                pData->y = (gyYDps * M_PI) / 180.0f;
                pData->z = (gyZDps * M_PI) / 180.0f;

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