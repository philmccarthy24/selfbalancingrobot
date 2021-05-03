#include "FXOS8700Mag.h"
#include "II2CDevice.h"
#include "AHRS.h"
#include "SBRCommon.h"
#include "spdlog/spdlog.h"
#include <cmath>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace sbrcontroller::sensors;

namespace sbrcontroller {
    namespace ahrs {
        namespace sensors {

            const float FXOS8700Mag::MAG_UT_LSB = 0.1F;

            FXOS8700Mag::FXOS8700Mag(std::shared_ptr<coms::II2CDevice> pI2CDevice, const sbrcontroller::sensors::TripleAxisData& hardIronOffset, std::shared_ptr<spdlog::logger> pLogger) :
                m_pFXOS8700(pI2CDevice),
                m_hardIronOffset {hardIronOffset},
                m_pLogger(pLogger)
            {
                // Make sure we have the correct chip ID since this checks
                // for correct address and that the IC is properly connected
                uint8_t id = m_pFXOS8700->ReadReg8(FXOS8700_REGISTER_WHO_AM_I);
                if (id != FXOS8700_ID) {
                    throw errorhandling::InvalidDeviceException(fmt::format("Expected id {:0x} but received id {:0x}", (int)FXOS8700_ID, id));
                }

                m_pLogger->debug("Using Hard Iron Offset {}, {}, {}", m_hardIronOffset.x, m_hardIronOffset.y, m_hardIronOffset.z);

                // Configure the magnetometer
                // Hybrid Mode, Over Sampling Rate = 16
                m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_MCTRL_REG1, 0x1F);
                // Jump to reg 0x33 after reading 0x06
                m_pFXOS8700->WriteReg8(FXOS8700_REGISTER_MCTRL_REG2, 0x20);
            }

            FXOS8700Mag::~FXOS8700Mag()
            {
            }

            SensorInfo FXOS8700Mag::GetDeviceInfo()
            {
                SensorInfo si = {};
                si.sensorType = ESensorType::Magnetometer;
                si.dataType = ESensorDataType::TripleAxis_MagneticFieldStrength_MicroTesla;
                si.requiredDataLength = sizeof(TripleAxisData);
                si.identifier = "FXOS8700 Magnetometer device";
                return si;
            }

            void FXOS8700Mag::ClearCalibration()
            {
                m_hardIronOffset = {0.0};
            }
            
            int FXOS8700Mag::ReadSensorData(unsigned char* buffer, unsigned int length)
            {
                if (length < sizeof(TripleAxisData))
                    return 0;

                // read raw counts from magnetometer
                short magXRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_MOUT_X_MSB));
                short magYRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_MOUT_Y_MSB));
                short magZRawCounts = static_cast<short>(m_pFXOS8700->ReadReg16(FXOS8700_REGISTER_MOUT_Z_MSB));
                // convert to units
                auto pData = reinterpret_cast<TripleAxisData*>(buffer);
                pData->x = static_cast<float>(magXRawCounts) * MAG_UT_LSB;
                pData->y = static_cast<float>(magYRawCounts) * MAG_UT_LSB;
                pData->z = static_cast<float>(magZRawCounts) * MAG_UT_LSB;

                // apply calibration
                pData->x -= m_hardIronOffset.x;
                pData->y -= m_hardIronOffset.y;
                pData->z -= m_hardIronOffset.z;
                
                return sizeof(TripleAxisData);
            }

        }
    }
}