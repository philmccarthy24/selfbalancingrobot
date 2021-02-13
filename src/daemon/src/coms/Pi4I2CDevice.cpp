#include "Pi4I2CDevice.h"
#include "sbrcontroller.h"
// headers required for RPi Buster i2c use
//https://www.kernel.org/doc/Documentation/i2c/dev-interface
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

namespace sbrcontroller {
    namespace coms {

        Pi4I2CDevice::Pi4I2CDevice(int deviceId) :
            II2CDevice(deviceId)
        {
            // RAII pattern: open device, if we can't, throw exception 
            // and fail to open
            //m_fdI2CDevice
            m_fdI2CDevice = open("/dev/i2c-1", O_RDWR);
            if (m_fdI2CDevice < 0) {
                throw errorhandling::ComsException("Failed to open I2C bus adapter");
            }
 
            if (ioctl(m_fdI2CDevice, I2C_SLAVE, deviceId) < 0) {
                throw errorhandling::ComsException("Failed to set slave device address");
            }
        }

        Pi4I2CDevice::~Pi4I2CDevice()
        {
            if (m_fdI2CDevice != -1) {
                close(m_fdI2CDevice);
            }
        }

        unsigned char Pi4I2CDevice::ReadReg8(int i2creg)
        {
            return i2c_smbus_read_byte_data(m_fdI2CDevice, i2creg);
        }

        void Pi4I2CDevice::WriteReg8(int i2creg, unsigned char data)
        {
            if (i2c_smbus_write_byte_data(m_fdI2CDevice, i2creg, data) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

        unsigned short Pi4I2CDevice::ReadReg16(int i2creg)
        {
            unsigned short reg16ns = i2c_smbus_read_word_data(m_fdI2CDevice, i2creg);
            unsigned short reg16 = ((reg16ns << 8) | (reg16ns >> 8)); // reverse the bytes
            return reg16;
        }

        void Pi4I2CDevice::WriteReg16(int i2creg, unsigned short data)
        {
            if (i2c_smbus_write_word_data(m_fdI2CDevice, i2creg, data) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

        std::vector<unsigned char> Pi4I2CDevice::ReadBlock(int i2creg)
        {
            //function reads at most 32 bytes
            std::vector<unsigned char> databuf;
            databuf.reserve(32);
            if (i2c_smbus_read_block_data(m_fdI2CDevice, i2creg, &databuf[0]) < 0) {
                throw errorhandling::ComsException("Failed to read data from I2C device");
            }
            return databuf;
        }

        void Pi4I2CDevice::WriteBlock(int i2creg, const std::vector<unsigned char>& buffer)
        {
            //function reads at most 32 bytes
            if (i2c_smbus_write_block_data(m_fdI2CDevice, i2creg, buffer.size(), &buffer[0]) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

    }
}