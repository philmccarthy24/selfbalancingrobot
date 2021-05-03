#include "GCodeChecksumCalc.h"
#include "SBRCommon.h"
#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace sbrcontroller;

namespace sbrcontroller {
    namespace test {

class GCodeChecksumCalcTests : public ::testing::Test
{
protected:

    virtual void SetUp()
    {      
    }

    virtual void TearDown()
    { 
    }
};


TEST_F(GCodeChecksumCalcTests, CheckCorrectChecksum)
{
    sbrcontroller::utility::GCodeChecksumCalc calc;

    auto checksum = calc.Calculate("r vbus_voltage ");

    ASSERT_EQ(checksum, "*93");
}

    }
}