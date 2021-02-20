#include "LinuxSerialDevice.h"
#include "sbrcontroller.h"
#include <cstring>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <map>

namespace sbrcontroller {
    namespace coms {

        LinuxSerialDevice::LinuxSerialDevice(const std::string& serialDeviceName, int baudRate)
        {
            m_fdSerialPort = open(serialDeviceName.c_str(), O_RDWR);
            if (m_fdSerialPort < 0) {
                throw errorhandling::ComsException(fmt::format("Could not open serial device. Error code {:d} - {}", errno, strerror(errno)));
            }
            // get current coms options
            termios tty = {};
            if(tcgetattr(m_fdSerialPort, &tty) != 0) {
                throw errorhandling::ComsException(fmt::format("Error {:d} from tcgetattr: {}", errno, strerror(errno)));
            }
            tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
            tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
            tty.c_cflag |= CS8; // 8 bits per byte (most common)
            tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
            tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
            tty.c_lflag &= ~ICANON;
            tty.c_lflag &= ~ECHO; // Disable echo
            tty.c_lflag &= ~ECHOE; // Disable erasure
            tty.c_lflag &= ~ECHONL; // Disable new-line echo
            tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
            tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
            tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
            tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
            tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
            // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
            // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

            tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
            tty.c_cc[VMIN] = 0;

            // Set in/out baud rate
            static const std::map<int, speed_t> supportedBaudLookup = std::map<int, speed_t> {
                {9600, B9600},
                {19200, B19200},
                {38400, B38400},
                {57600, B57600},
                {115200, B115200}
            };
            auto supportedBaud = supportedBaudLookup.find(baudRate);
            if (supportedBaud == supportedBaudLookup.end()) {
                std::vector<int> supportedBauds;
                for(auto const& imap: supportedBaudLookup)
                    supportedBauds.push_back(imap.first);
                throw errorhandling::ComsException(fmt::format("Baud rate {:d} not supported; choose from '{}'", baudRate, fmt::join(supportedBauds, ", ")));
            }
            cfsetspeed(&tty, supportedBaud->second);

            if (tcsetattr(m_fdSerialPort, TCSANOW, &tty) != 0) {
                throw errorhandling::ComsException(fmt::format("Error {:d} from tcsetattr: {}", errno, strerror(errno)));
            }
        }

        LinuxSerialDevice::~LinuxSerialDevice()
        {
            close(m_fdSerialPort);
        }

        int LinuxSerialDevice::Read(char* bufferToRead, int bufLen) const
        {
            int numBytesRead = read(m_fdSerialPort, bufferToRead, bufLen);
            if (numBytesRead < 0) {
                throw errorhandling::ComsException(fmt::format("Error {:d} from read: {}", errno, strerror(errno)));
            }
            return numBytesRead;
        }

        int LinuxSerialDevice::Write(char* bufferToWrite, int bufLen) const
        {
            int numBytesWritten = write(m_fdSerialPort, bufferToWrite, bufLen);
            if (numBytesWritten < 0) {
                throw errorhandling::ComsException(fmt::format("Error {:d} from write: {}", errno, strerror(errno)));
            }
            return numBytesWritten;
        }

    }

}