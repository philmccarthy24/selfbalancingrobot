#include "LinuxI2CDevice.h"
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

        LinuxI2CDevice::LinuxI2CDevice(int deviceId) :
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

        LinuxI2CDevice::~LinuxI2CDevice()
        {
            if (m_fdI2CDevice != -1) {
                close(m_fdI2CDevice);
            }
        }

        unsigned char LinuxI2CDevice::ReadReg8(int i2creg)
        {
            return i2c_smbus_read_byte_data(m_fdI2CDevice, i2creg);
        }

        void LinuxI2CDevice::WriteReg8(int i2creg, unsigned char data)
        {
            if (i2c_smbus_write_byte_data(m_fdI2CDevice, i2creg, data) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

        unsigned short LinuxI2CDevice::ReadReg16(int i2creg)
        {
            unsigned short reg16ns = i2c_smbus_read_word_data(m_fdI2CDevice, i2creg);
            unsigned short reg16 = ((reg16ns << 8) | (reg16ns >> 8)); // reverse the bytes
            return reg16;
        }

        void LinuxI2CDevice::WriteReg16(int i2creg, unsigned short data)
        {
            if (i2c_smbus_write_word_data(m_fdI2CDevice, i2creg, data) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

        std::vector<unsigned char> LinuxI2CDevice::ReadBlock(int i2creg)
        {
            //function reads at most 32 bytes
            std::vector<unsigned char> databuf;
            databuf.reserve(32);
            if (i2c_smbus_read_block_data(m_fdI2CDevice, i2creg, &databuf[0]) < 0) {
                throw errorhandling::ComsException("Failed to read data from I2C device");
            }
            return databuf;
        }

        void LinuxI2CDevice::WriteBlock(int i2creg, const std::vector<unsigned char>& buffer)
        {
            //function reads at most 32 bytes
            if (i2c_smbus_write_block_data(m_fdI2CDevice, i2creg, buffer.size(), &buffer[0]) < 0) {
                throw errorhandling::ComsException("Failed to write data to I2C device");
            }
        }

        /*
        I don't know if the block reads/writes above work, eg the read doesn't read n bytes.

        Here is an example from https://stackoverflow.com/questions/55976683/read-a-block-of-data-from-a-specific-registerfifo-using-c-c-and-i2c-in-raspb
        that uses raw ioctls to do block reads/writes (not tested, to try at some point):

        static inline int i2c_rdwr_block(int fd, uint8_t reg, uint8_t read_write, uint8_t length, unsigned char* buffer)
        {
            struct i2c_smbus_ioctl_data ioctl_data;
            union i2c_smbus_data smbus_data;

            int rv; 

            if(length > I2C_SMBUS_BLOCK_MAX) 
            {
                std::cerr << "Requested Length is greater than the maximum specified" << std::endl;
                return -1;
            }

            // First byte is always the size to write and to receive 
            // https://github.com/torvalds/linux/blob/master/drivers/i2c/i2c-core-smbus.c  
            // (See i2c_smbus_xfer_emulated CASE:I2C_SMBUS_I2C_BLOCK_DATA)
            smbus_data.block[0] = length;

            if ( read_write != I2C_SMBUS_READ )
            {
                for(int i = 0; i < length; i++)
                {
                    smbus_data.block[i + 1] = buffer[i];
                }
            }

            ioctl_data.read_write = read_write;
            ioctl_data.command = reg;
            ioctl_data.size = I2C_SMBUS_I2C_BLOCK_DATA;
            ioctl_data.data = &smbus_data;

            rv = ioctl (fd, I2C_SMBUS, &ioctl_data);
            if (rv < 0)
            {
                std::cerr << "Accessing I2C Read/Write failed! Error is: " << strerror(errno) << std::endl;
                return rv;
            }

            if (read_write == I2C_SMBUS_READ)
            {
                for(int i = 0; i < length; i++)
                {
                    // Skip the first byte, which is the length of the rest of the block.
                    buffer[i] = smbus_data.block[i+1];
                }
            }

            return rv;
        }

        */

    }
}