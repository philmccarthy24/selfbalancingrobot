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

        static std::shared_ptr<sbrcontroller::sensors::ISensor> GetSensorByType(const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors, sbrcontroller::sensors::ESensorType sensorType);

        void PerformGyroCalibration();
        void PerformAccelerometerCalibration();
        void PerformMagnetometerCalibration();
        const sbrcontroller::sensors::TripleAxisData& GetGyroCalibration();
        const sbrcontroller::sensors::TripleAxisData& GetMagnetometerCalibration();
        const sbrcontroller::sensors::TripleAxisData& GetAccelerometerCalibration();

    private:
        std::shared_ptr<sbrcontroller::sensors::ISensor> m_pGyroToCalibrate;
        std::shared_ptr<sbrcontroller::sensors::ISensor> m_pMagnetometerToCalibrate;
        std::shared_ptr<sbrcontroller::sensors::ISensor> m_pAccelerometerToCalibrate;

        sbrcontroller::sensors::TripleAxisData m_zeroRateOffset;
        sbrcontroller::sensors::TripleAxisData m_accelZeroRateOffset;
        sbrcontroller::sensors::TripleAxisData m_hardIronOffset;
    };

}