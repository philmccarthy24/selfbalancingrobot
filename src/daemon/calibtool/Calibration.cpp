#include "Calibration.h"
#include "JSONConfig.h"
#include "SBRCommon.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <fmt/core.h>

namespace calibtool {

    Calibration::Calibration(const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors)
    {
        m_pGyroToCalibrate = GetSensorByType(sensors, sbrcontroller::sensors::ESensorType::Gyroscope);
        m_pAccelerometerToCalibrate = GetSensorByType(sensors, sbrcontroller::sensors::ESensorType::Accelerometer);
        m_pMagnetometerToCalibrate = GetSensorByType(sensors, sbrcontroller::sensors::ESensorType::Magnetometer);
        m_pGyroToCalibrate->ClearCalibration();
        m_pAccelerometerToCalibrate->ClearCalibration();
        m_pMagnetometerToCalibrate->ClearCalibration();
    }

    Calibration::~Calibration()
    {
    }

    std::shared_ptr<sbrcontroller::sensors::ISensor> Calibration::GetSensorByType(const std::vector<std::shared_ptr<sbrcontroller::sensors::ISensor>>& sensors, sbrcontroller::sensors::ESensorType sensorType)
    {
        auto sensorIter = std::find_if(sensors.begin(), sensors.end(), [&] (const std::shared_ptr<sbrcontroller::sensors::ISensor>& item) {
            return item->GetDeviceInfo().sensorType == sensorType;
        });
        if (sensorIter == sensors.end())
            throw std::runtime_error("Can't identify sensor of specified type");
        return *sensorIter;
    }

    void Calibration::PerformGyroCalibration()
    {
        printf("Gyro calibration\n");
        printf("Place your gyro on a FLAT stable surface\n");
        printf("Press ENTER to continue");
        getchar();

        sbrcontroller::sensors::TripleAxisData gyroReading = {};
        float minX = 0, maxX = 0;
        float minY = 0, maxY = 0;
        float minZ = 0, maxZ = 0;

        for (int i = 0; i < 500; i++)
        {
            m_pGyroToCalibrate->ReadSensorData(reinterpret_cast<unsigned char*>(&gyroReading), sizeof(sbrcontroller::sensors::TripleAxisData));

            printf(fmt::format("Gyro: x={}, y={}, z={}\n", gyroReading.x, gyroReading.y, gyroReading.z).c_str());

            minX = std::min(minX, gyroReading.x);
            minY = std::min(minY, gyroReading.y);
            minZ = std::min(minZ, gyroReading.z);
            maxX = std::max(maxX, gyroReading.x);
            maxY = std::max(maxY, gyroReading.y);
            maxZ = std::max(maxZ, gyroReading.z);

            m_zeroRateOffset.x = (minX + maxX) / 2;
            m_zeroRateOffset.y = (minY + maxY) / 2;
            m_zeroRateOffset.z = (minZ + maxZ) / 2;

            printf(fmt::format("Zero Rate Offset: x={}, y={}, z={}\n", m_zeroRateOffset.x, m_zeroRateOffset.y, m_zeroRateOffset.z).c_str());

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void Calibration::PerformAccelerometerCalibration()
    {
        printf("Accelerometer calibration\n");
        printf("Place your accelerometer on a FLAT stable surface\n");
        printf("Press ENTER to continue");
        getchar();

        sbrcontroller::sensors::TripleAxisData accelReading = {};
        float minX = 0, maxX = 0;
        float minY = 0, maxY = 0;
        float minZ = 0, maxZ = 0;

        for (int i = 0; i < 500; i++)
        {
            m_pAccelerometerToCalibrate->ReadSensorData(reinterpret_cast<unsigned char*>(&accelReading), sizeof(sbrcontroller::sensors::TripleAxisData));

            printf(fmt::format("Accel: x={}, y={}, z={}\n", accelReading.x, accelReading.y, accelReading.z).c_str());

            minX = std::min(minX, accelReading.x);
            minY = std::min(minY, accelReading.y);
            minZ = std::min(minZ, accelReading.z - 1.0f);
            maxX = std::max(maxX, accelReading.x);
            maxY = std::max(maxY, accelReading.y);
            maxZ = std::max(maxZ, accelReading.z - 1.0f);

            m_accelZeroRateOffset.x = (minX + maxX) / 2;
            m_accelZeroRateOffset.y = (minY + maxY) / 2;
            m_accelZeroRateOffset.z = (minZ + maxZ) / 2;

            printf(fmt::format("Zero Rate Offset: x={}, y={}, z={}\n", m_accelZeroRateOffset.x, m_accelZeroRateOffset.y, m_accelZeroRateOffset.z).c_str());

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void Calibration::PerformMagnetometerCalibration()
    {
        // Do this in a thread so we can wait for user input, 
        // then trigger exit via atomic bool state
        printf("Magnetometer calibration\n");
        printf("Rotate your magnetometer slowly around until the hard iron offset values stop moving, then press ENTER.\n");
        printf("Press ENTER to continue");
        getchar();
        
        std::atomic_bool calibComplete;
        calibComplete.store(false);
        std::thread magCalThread([&calibComplete, this] () 
        {
            sbrcontroller::sensors::TripleAxisData magReading = {};
            float minX = 0, maxX = 0;
            float minY = 0, maxY = 0;
            float minZ = 0, maxZ = 0;

            while (calibComplete == false)
            {
                m_pMagnetometerToCalibrate->ReadSensorData(reinterpret_cast<unsigned char*>(&magReading), sizeof(sbrcontroller::sensors::TripleAxisData));

                //printf(fmt::format("Magnetometer: x={}, y={}, z={}\n", magReading.x, magReading.y, magReading.z).c_str());

                minX = std::min(minX, magReading.x);
                minY = std::min(minY, magReading.y);
                minZ = std::min(minZ, magReading.z);
                maxX = std::max(maxX, magReading.x);
                maxY = std::max(maxY, magReading.y);
                maxZ = std::max(maxZ, magReading.z);

                m_hardIronOffset.x = (minX + maxX) / 2;
                m_hardIronOffset.y = (minY + maxY) / 2;
                m_hardIronOffset.z = (minZ + maxZ) / 2;

                printf(fmt::format("Hard Iron Offset: x={}, y={}, z={}\n", m_hardIronOffset.x, m_hardIronOffset.y, m_hardIronOffset.z).c_str());

                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });

        getchar();
        calibComplete.store(true);
        magCalThread.join();
    }

    const sbrcontroller::sensors::TripleAxisData& Calibration::GetGyroCalibration()
    {
        return m_zeroRateOffset;
    }

    const sbrcontroller::sensors::TripleAxisData& Calibration::GetAccelerometerCalibration()
    {
        return m_accelZeroRateOffset;
    }

    const sbrcontroller::sensors::TripleAxisData& Calibration::GetMagnetometerCalibration()
    {
        return m_hardIronOffset;
    }
}