#include "Pi4I2CDevice.h"
#include "sbrcontroller.h"

namespace sbrcontroller {
    namespace coms {

        Pi4I2CDevice::Pi4I2CDevice(int deviceId) :
            II2CDevice(deviceId)
        {
            // RAII pattern: open device, if we can't, throw exception 
            // and fail to open
            //m_fdI2CDevice
        }

        Pi4I2CDevice::~Pi4I2CDevice()
        {

        }

        unsigned char Pi4I2CDevice::ReadReg8(int i2creg)
        {
            throw errorhandling::NotImplementedException("Not yet implemented.");
        }

        void Pi4I2CDevice::WriteReg8(int i2creg, unsigned char data)
        {

        }

        unsigned short Pi4I2CDevice::ReadReg16(int i2creg)
        {
            throw errorhandling::NotImplementedException("Not yet implemented.");
        }

        void Pi4I2CDevice::WriteReg16(int i2creg, unsigned short data)
        {

        }

        std::vector<unsigned char> Pi4I2CDevice::ReadBlock(int i2creg)
        {
            throw errorhandling::NotImplementedException("Not yet implemented.");
        }

        void Pi4I2CDevice::WriteBlock(int i2creg, const std::vector<unsigned char>& buffer)
        {

        }

    }
}