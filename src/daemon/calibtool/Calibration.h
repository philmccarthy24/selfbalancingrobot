#pragma once

#include "sensors.h"
#include <memory>
#include <vector>
#include <numeric>


namespace calibtool {

    class Calibration
    {
    public:
        Calibration(const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors);
        ~Calibration();

        void PerformGyroCalibration();
        void PerformMagnetometerCalibration();
        const sbrcontroller::sensors::TripleAxisData& GetGyroCalibration();
        const sbrcontroller::sensors::TripleAxisData& GetMagnetometerCalibration();

    private:
        std::shared_ptr<sbrcontroller::sensors::ISensor> m_pGyroToCalibrate;
        std::shared_ptr<sbrcontroller::sensors::ISensor> m_pMagnetometerToCalibrate;

        sbrcontroller::sensors::TripleAxisData m_zeroRateOffset;
        sbrcontroller::sensors::TripleAxisData m_hardIronOffset;
    };

}