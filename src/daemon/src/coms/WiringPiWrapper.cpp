#include "WiringPiWrapper.h"
#include "sbrcontroller.h"
#include "wiringPi.h"

namespace sbrcontroller {
    namespace coms {

        WiringPiWrapper::WiringPiWrapper(int deviceId)
        {
            // RAII pattern: open device, if we can't, throw exception 
            // and fail to open
            m_fdI2CDevice = wiringPiI2CSetup(deviceId);
            if (m_fdI2CDevice == -1) 
            {
                throw ComsException("Wiring PI Could not setup MPU6050 I2C coms");
            }
        }

        WiringPiWrapper::~WiringPiWrapper()
        {
        }

        unsigned char WiringPiWrapper::ReadReg8(int register)
        {
            return static_cast<unsigned char>(wiringPiI2CReadReg8(m_fdI2CDevice, register);
        }

        void WiringPiWrapper::WriteReg8(int register, unsigned char data)
        {
            wiringPiI2CWriteReg8(m_fdI2CDevice, register, data);
        }

        // TODO: understand how this compares to reading a reg16.
        unsigned short WiringPiWrapper::ReadReg16(int register)
        {
            int highByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister);
            int lowByte = wiringPiI2CReadReg8(m_fdMPU6050, highRegister + 1);
            return (highByte << 8) | lowByte;
        }

        void WiringPiWrapper::WriteReg16(int register, unsigned short data)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::WriteReg16 not implemented");
        }

        std::vector<unsigned char> WiringPiWrapper::ReadBlock(int register)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::ReadBlock not implemented");
        }

        void WiringPiWrapper::WriteBlock(int register, const std::vector<unsigned char>& buffer)
        {
            throw errorhandling::NotImplementedException("WiringPiWrapper::WriteBlock not implemented");
        }

    }
}