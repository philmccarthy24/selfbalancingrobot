#ifdef USING_WIRING_PI

#include "WiringPiWrapper.h"
#include "sbrcontroller.h"
#include "wiringPiI2C.h"

namespace sbrcontroller {
    namespace coms {

        WiringPiWrapper::WiringPiWrapper(int deviceId) : 
            II2CDevice(deviceId)
        {
            // RAII pattern: open device, if we can't, throw exception 
            // and fail to open
            m_fdI2CDevice = wiringPiI2CSetup(deviceId);
            if (m_fdI2CDevice == -1) 
            {
                throw errorhandling::ComsException("Wiring PI Could not setup MPU6050 I2C coms");
            }
        }

        WiringPiWrapper::~WiringPiWrapper()
        {
        }

        unsigned char WiringPiWrapper::ReadReg8(int i2creg)
        {
            return static_cast<unsigned char>(wiringPiI2CReadReg8(m_fdI2CDevice, i2creg));
        }

        void WiringPiWrapper::WriteReg8(int i2creg, unsigned char data)
        {
            wiringPiI2CWriteReg8(m_fdI2CDevice, i2creg, data);
        }

        // TODO: understand how this compares to reading a reg16.
        unsigned short WiringPiWrapper::ReadReg16(int i2creg)
        {
            unsigned short reg16ns = static_cast<unsigned short>(wiringPiI2CReadReg16(m_fdI2CDevice, i2creg));
            unsigned short reg16 = ((reg16ns << 8) | (reg16ns >> 8)); // reverse the bytes
            return reg16;
        }

        void WiringPiWrapper::WriteReg16(int i2creg, unsigned short data)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::WriteReg16 not implemented");
        }

        std::vector<unsigned char> WiringPiWrapper::ReadBlock(int i2creg)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::ReadBlock not implemented");
        }

        void WiringPiWrapper::WriteBlock(int i2creg, const std::vector<unsigned char>& buffer)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::WriteBlock not implemented");
        }

    }
}

#endif