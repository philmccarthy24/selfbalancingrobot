#include "StringReaderWriter.h"
#include "ISerialDevice.h"
#include "sbrcontroller.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>

using namespace std;
using namespace sbrcontroller;
using ::testing::Return;
using ::testing::_;

namespace sbrcontroller {
    namespace test {

class MockSerialDevice : public sbrcontroller::coms::ISerialDevice 
{
 public:
  MOCK_METHOD(int, Read, (char* bufferToRead, int bufLen), (const, override));
  MOCK_METHOD(int, Write, (char* bufferToWrite, int bufLen), (const, override));
};

class StringReaderWriterTests : public ::testing::Test
{
protected:

    virtual void SetUp()
    {      
    }

    virtual void TearDown()
    { 
    }
};


TEST_F(StringReaderWriterTests, BasicRead)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Read(_, _))
    .Times(1)
    .WillOnce([&] (char* bufferToRead, int bufLen) {
        std:string result = "Happy cow\n";
        if (bufLen < result.size())
            throw std::runtime_error("Buffer not big enough");
        strcpy(bufferToRead, result.c_str());
        return result.size(); 
    });

    coms::StringReaderWriter srw(pSerialDeviceMock);

    ASSERT_EQ(srw.Read('\n'), "Happy cow\n");
}

TEST_F(StringReaderWriterTests, BasicReadWithUnderflow)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Read(_, _))
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        std:string result = "Happy ";
        if (bufLen < result.size())
            throw std::runtime_error("Buffer not big enough");
        strcpy(bufferToRead, result.c_str());
        return result.size();
    })
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        std:string result = "Clappy Cow\n";
        if (bufLen < result.size())
            throw std::runtime_error("Buffer not big enough");
        strcpy(bufferToRead, result.c_str());
        return result.size();
    });

    coms::StringReaderWriter srw(pSerialDeviceMock);

    ASSERT_EQ(srw.Read('\n'), "Happy Clappy Cow\n");
}

TEST_F(StringReaderWriterTests, ReadWithUnderflowAndSmallReadBuffer)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Read(_, _))
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        if (bufLen != 5)
            throw std::runtime_error("Buffer unexpected length");
        std:string result = "Tiny";
        strcpy(bufferToRead, result.c_str());
        return result.size();
    })
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        if (bufLen != 1)
            throw std::runtime_error("Buffer unexpected length");
        std:string result = " ";
        strcpy(bufferToRead, result.c_str());
        return result.size();
    })
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        if (bufLen != 5)
            throw std::runtime_error("Buffer unexpected length");
        std:string result = "Cow\n";
        strcpy(bufferToRead, result.c_str());
        return result.size();
    });

    // 5 character read buffer!
    coms::StringReaderWriter srw(pSerialDeviceMock, 5, 5000);

    ASSERT_EQ(srw.Read('\n'), "Tiny Cow\n");
}

TEST_F(StringReaderWriterTests, SlowReadTimesOut)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Read(_, _))
    .WillOnce([&] (char* bufferToRead, int bufLen) -> int {
        std:string result = "Happy ";
        if (bufLen < result.size())
            throw std::runtime_error("Buffer not big enough");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        strcpy(bufferToRead, result.c_str());
        return result.size();
    });

    coms::StringReaderWriter srw(pSerialDeviceMock, 1024, 200);

    EXPECT_THROW(srw.Read('\n'), errorhandling::ComsException);
}

TEST_F(StringReaderWriterTests, BasicWrite)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Write(_, _))
    .WillOnce(Return(5))
    .WillOnce(Return(4))
    .WillOnce(Return(3));

    coms::StringReaderWriter srw(pSerialDeviceMock);

    srw.Write("Hello world\n");
}

TEST_F(StringReaderWriterTests, SlowWriteTimesOut)
{
    auto pSerialDeviceMock = std::make_shared<MockSerialDevice>();

    EXPECT_CALL(*pSerialDeviceMock, Write(_, _))
    .WillOnce([&] (char* bufferToWrite, int bufLen) -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return bufLen;
    });

    coms::StringReaderWriter srw(pSerialDeviceMock, 1024, 200);

    EXPECT_THROW(srw.Write("In the slow lane\n"), errorhandling::ComsException);
}

    }
}