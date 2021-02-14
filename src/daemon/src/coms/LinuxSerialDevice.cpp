#include "LinuxSerialDevice.h"
#include "sbrcontroller.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <map>

namespace sbrcontroller {
    namespace coms {

        LinuxSerialDevice::LinuxSerialDevice(const std::string& serialDeviceName, int baudRate) :
            m_bKillSignal(ATOMIC_VAR_INIT(false))
        {
            m_tComsThread = std::thread([serialDeviceName,baudRate,this] {
                SerialComsThreadProc(serialDeviceName, baudRate);
            });
        }

        LinuxSerialDevice::~LinuxSerialDevice()
        {
            m_bKillSignal.store(true);
            m_tComsThread.join();
        }

        void LinuxSerialDevice::SerialComsThreadProc(const std::string& serialDeviceName, int baudRate)
        {
            int fdSerialPort = open(serialDeviceName.c_str(), O_RDWR);
            if (fdSerialPort < 0) {
                throw errorhandling::ComsException(fmt::format("Could not open serial device. Error code {:d} - {}", errno, strerror(errno)));
            }
            // get current coms options
            termios tty = {};
            if(tcgetattr(fdSerialPort, &tty) != 0) {
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
            static const auto supportedBaudLookup = std::map<int, speed_t> {
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

            if (tcsetattr(fdSerialPort, TCSANOW, &tty) != 0) {
                throw errorhandling::ComsException(fmt::format("Error {:d} from tcsetattr: {}", errno, strerror(errno)));
            }

/*
            while (!m_bKillSignal.load())
            {
                m_ioQueue.empty
                int n = read(fdSerialPort, &read_buf, sizeof(read_buf));
            }*/
            // test doing a write here
            std::string command = "v 0 1 0\n";
            std::vector<unsigned char> writeBuf(command.begin(), command.end());
            write(fdSerialPort, writeBuf.data(), writeBuf.size());

            close(fdSerialPort);
        }

        std::future<std::vector<unsigned char>> LinuxSerialDevice::ReadAsync(int numBytesToRead) const
        {
            throw errorhandling::NotImplementedException("Not implemented");
        }

        std::future<void> LinuxSerialDevice::WriteAsync(const std::vector<unsigned char>& bytesToWrite) const
        {
            throw errorhandling::NotImplementedException("Not implemented");
        }
    }

}