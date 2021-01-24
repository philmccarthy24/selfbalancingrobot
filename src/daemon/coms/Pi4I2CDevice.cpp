#include "Pi4I2CDevice.h"

namespace sbrcontroller {
    namespace coms {

        Pi4I2CDevice::Pi4I2CDevice(int deviceId)
        {
            // RAII pattern: open device, if we can't, throw exception 
            // and fail to open
            m_fdI2CDevice
        }

        Pi4I2CDevice::~Pi4I2CDevice()
        {

        }

        unsigned char Pi4I2CDevice::ReadReg8(int register)
        {

        }

        void Pi4I2CDevice::WriteReg8(int register, unsigned char data)
        {

        }

        unsigned short Pi4I2CDevice::ReadReg16(int register)
        {

        }

        void Pi4I2CDevice::WriteReg16(int register, unsigned short data)
        {

        }

        std::vector<unsigned char> Pi4I2CDevice::ReadBlock(int register)
        {

        }

        void Pi4I2CDevice::WriteBlock(int register, const std::vector<unsigned char>& buffer)
        {

        }

    }
}