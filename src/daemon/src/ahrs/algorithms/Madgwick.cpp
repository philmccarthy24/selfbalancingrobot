//=====================================================================================================
// Madgwick.cpp
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
// 23/01/2021   Phil McCarthy   Ported to C++
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "Madgwick.h"
#include <cmath>

namespace sbrcontroller {
	namespace ahrs {
		namespace algorithms {

			Madgwick::Madgwick(float sampleFreq, float betaConst) :
				beta(betaConst),
				m_fSampleFreqHz(sampleFreq),
				m_q{1.0f, 0.0f, 0.0f, 0.0f}
			{
				m_bSignalRequestReading.store(false);
			}

			Madgwick::~Madgwick()
			{
			}

			bool Madgwick::IsHardwareImplementation()
			{
				return false;
			}

			void Madgwick::Update(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData, const sensors::TripleAxisData& magData)
			{
				float recipNorm;
				float s0, s1, s2, s3;
				float qDot1, qDot2, qDot3, qDot4;
				float hx, hy;
				float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

				// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
				if((magData.x == 0.0f) && (magData.y == 0.0f) && (magData.z == 0.0f)) {
					UpdateIMU(gyroData, accelData);
					return;
				}

				sensors::TripleAxisData acc = accelData;
				sensors::TripleAxisData mag = magData;

				// Rate of change of quaternion from gyroscope
				qDot1 = 0.5f * (-m_q.x * gyroData.x - m_q.y * gyroData.y - m_q.z * gyroData.z);
				qDot2 = 0.5f * (m_q.w * gyroData.x + m_q.y * gyroData.z - m_q.z * gyroData.y);
				qDot3 = 0.5f * (m_q.w * gyroData.y - m_q.x * gyroData.z + m_q.z * gyroData.x);
				qDot4 = 0.5f * (m_q.w * gyroData.z + m_q.x * gyroData.y - m_q.y * gyroData.x);

				// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
				if(!((acc.x == 0.0f) && (acc.y == 0.0f) && (acc.z == 0.0f))) {

					// Normalise accelerometer measurement
					recipNorm = invSqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);
					acc.x *= recipNorm;
					acc.y *= recipNorm;
					acc.z *= recipNorm;   

					// Normalise magnetometer measurement
					recipNorm = invSqrt(magData.x * magData.x + magData.y * magData.y + magData.z * magData.z);
					mag.x *= recipNorm;
					mag.y *= recipNorm;
					mag.z *= recipNorm;

					// Auxiliary variables to avoid repeated arithmetic
					_2q0mx = 2.0f * m_q.w * mag.x;
					_2q0my = 2.0f * m_q.w * mag.y;
					_2q0mz = 2.0f * m_q.w * mag.z;
					_2q1mx = 2.0f * m_q.x * mag.x;
					_2q0 = 2.0f * m_q.w;
					_2q1 = 2.0f * m_q.x;
					_2q2 = 2.0f * m_q.y;
					_2q3 = 2.0f * m_q.z;
					_2q0q2 = 2.0f * m_q.w * m_q.y;
					_2q2q3 = 2.0f * m_q.y * m_q.z;
					q0q0 = m_q.w * m_q.w;
					q0q1 = m_q.w * m_q.x;
					q0q2 = m_q.w * m_q.y;
					q0q3 = m_q.w * m_q.z;
					q1q1 = m_q.x * m_q.x;
					q1q2 = m_q.x * m_q.y;
					q1q3 = m_q.x * m_q.z;
					q2q2 = m_q.y * m_q.y;
					q2q3 = m_q.y * m_q.z;
					q3q3 = m_q.z * m_q.z;

					// Reference direction of Earth's magnetic field
					hx = mag.x * q0q0 - _2q0my * m_q.z + _2q0mz * m_q.y + mag.x * q1q1 + _2q1 * mag.y * m_q.y + _2q1 * mag.z * m_q.z - mag.x * q2q2 - mag.x * q3q3;
					hy = _2q0mx * m_q.z + mag.y * q0q0 - _2q0mz * m_q.x + _2q1mx * m_q.y - mag.y * q1q1 + mag.y * q2q2 + _2q2 * mag.z * m_q.z - mag.y * q3q3;
					_2bx = sqrtf(hx * hx + hy * hy);
					_2bz = -_2q0mx * m_q.y + _2q0my * m_q.x + mag.z * q0q0 + _2q1mx * m_q.z - mag.z * q1q1 + _2q2 * mag.y * m_q.z - mag.z * q2q2 + mag.z * q3q3;
					_4bx = 2.0f * _2bx;
					_4bz = 2.0f * _2bz;

					// Gradient decent algorithm corrective step
					s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - acc.x) + _2q1 * (2.0f * q0q1 + _2q2q3 - acc.y) - _2bz * m_q.y * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mag.x) + (-_2bx * m_q.z + _2bz * m_q.x) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - mag.y) + _2bx * m_q.y * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mag.z);
					s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - acc.x) + _2q0 * (2.0f * q0q1 + _2q2q3 - acc.y) - 4.0f * m_q.x * (1 - 2.0f * q1q1 - 2.0f * q2q2 - acc.z) + _2bz * m_q.z * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mag.x) + (_2bx * m_q.y + _2bz * m_q.w) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - mag.y) + (_2bx * m_q.z - _4bz * m_q.x) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mag.z);
					s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - acc.x) + _2q3 * (2.0f * q0q1 + _2q2q3 - acc.y) - 4.0f * m_q.y * (1 - 2.0f * q1q1 - 2.0f * q2q2 - acc.z) + (-_4bx * m_q.y - _2bz * m_q.w) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mag.x) + (_2bx * m_q.x + _2bz * m_q.z) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - mag.y) + (_2bx * m_q.w - _4bz * m_q.y) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mag.z);
					s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - acc.x) + _2q2 * (2.0f * q0q1 + _2q2q3 - acc.y) + (-_4bx * m_q.z + _2bz * m_q.x) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mag.x) + (-_2bx * m_q.w + _2bz * m_q.y) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - mag.y) + _2bx * m_q.x * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mag.z);
					recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
					s0 *= recipNorm;
					s1 *= recipNorm;
					s2 *= recipNorm;
					s3 *= recipNorm;

					// Apply feedback step
					qDot1 -= beta * s0;
					qDot2 -= beta * s1;
					qDot3 -= beta * s2;
					qDot4 -= beta * s3;
				}

				// Integrate rate of change of quaternion to yield quaternion
				float invSampleFreq = (1.0f / m_fSampleFreqHz);
				m_q.w += qDot1 * invSampleFreq;
				m_q.x += qDot2 * invSampleFreq;
				m_q.y += qDot3 * invSampleFreq;
				m_q.z += qDot4 * invSampleFreq;

				// Normalise quaternion
				recipNorm = invSqrt(m_q.w * m_q.w + m_q.x * m_q.x + m_q.y * m_q.y + m_q.z * m_q.z);
				m_q.w *= recipNorm;
				m_q.x *= recipNorm;
				m_q.y *= recipNorm;
				m_q.z *= recipNorm;

				if (m_bSignalRequestReading) {
					const std::lock_guard<std::mutex> lock(m_sensorFusionLock);
					Quaternion out { m_q.w, m_q.x, m_q.y, m_q.z };
					while (!m_readingPromises.empty())
  					{
						auto& readPromise = m_readingPromises.front();
						readPromise.set_value(out);
						m_readingPromises.pop();
						// note it is safe for promise to go out of scope here -
						// state is shared and active while associated future is live
					}
					m_bSignalRequestReading = false;
				}
			}

			//---------------------------------------------------------------------------------------------------
			// IMU algorithm update

			void Madgwick::UpdateIMU(const sensors::TripleAxisData& gyroData, const sensors::TripleAxisData& accelData)
			{
				float recipNorm;
				float s0, s1, s2, s3;
				float qDot1, qDot2, qDot3, qDot4;
				float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

				// Rate of change of quaternion from gyroscope
				qDot1 = 0.5f * (-m_q.x * gyroData.x - m_q.y * gyroData.y - m_q.z * gyroData.z);
				qDot2 = 0.5f * (m_q.w * gyroData.x + m_q.y * gyroData.z - m_q.z * gyroData.y);
				qDot3 = 0.5f * (m_q.w * gyroData.y - m_q.x * gyroData.z + m_q.z * gyroData.x);
				qDot4 = 0.5f * (m_q.w * gyroData.z + m_q.x * gyroData.y - m_q.y * gyroData.x);

				// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
				if(!((accelData.x == 0.0f) && (accelData.y == 0.0f) && (accelData.z == 0.0f))) {

					// Normalise accelerometer measurement
					sensors::TripleAxisData acc = accelData;
					recipNorm = invSqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);
					acc.x *= recipNorm;
					acc.y *= recipNorm;
					acc.z *= recipNorm;   

					// Auxiliary variables to avoid repeated arithmetic
					_2q0 = 2.0f * m_q.w;
					_2q1 = 2.0f * m_q.x;
					_2q2 = 2.0f * m_q.y;
					_2q3 = 2.0f * m_q.z;
					_4q0 = 4.0f * m_q.w;
					_4q1 = 4.0f * m_q.x;
					_4q2 = 4.0f * m_q.y;
					_8q1 = 8.0f * m_q.x;
					_8q2 = 8.0f * m_q.y;
					q0q0 = m_q.w * m_q.w;
					q1q1 = m_q.x * m_q.x;
					q2q2 = m_q.y * m_q.y;
					q3q3 = m_q.z * m_q.z;

					// Gradient decent algorithm corrective step
					s0 = _4q0 * q2q2 + _2q2 * acc.x + _4q0 * q1q1 - _2q1 * acc.y;
					s1 = _4q1 * q3q3 - _2q3 * acc.x + 4.0f * q0q0 * m_q.x - _2q0 * acc.y - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * acc.z;
					s2 = 4.0f * q0q0 * m_q.y + _2q0 * acc.x + _4q2 * q3q3 - _2q3 * acc.y - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * acc.z;
					s3 = 4.0f * q1q1 * m_q.z - _2q1 * acc.x + 4.0f * q2q2 * m_q.z - _2q2 * acc.y;
					recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
					s0 *= recipNorm;
					s1 *= recipNorm;
					s2 *= recipNorm;
					s3 *= recipNorm;

					// Apply feedback step
					qDot1 -= beta * s0;
					qDot2 -= beta * s1;
					qDot3 -= beta * s2;
					qDot4 -= beta * s3;
				}

				// Integrate rate of change of quaternion to yield quaternion
				float invSampleFreq = (1.0f / m_fSampleFreqHz);
				m_q.w += qDot1 * invSampleFreq;
				m_q.x += qDot2 * invSampleFreq;
				m_q.y += qDot3 * invSampleFreq;
				m_q.z += qDot4 * invSampleFreq;

				// Normalise quaternion
				recipNorm = invSqrt(m_q.w * m_q.w + m_q.x * m_q.x + m_q.y * m_q.y + m_q.z * m_q.z);
				m_q.w *= recipNorm;
				m_q.x *= recipNorm;
				m_q.y *= recipNorm;
				m_q.z *= recipNorm;

				if (m_bSignalRequestReading) {
					const std::lock_guard<std::mutex> lock(m_sensorFusionLock);
					Quaternion out { m_q.w, m_q.x, m_q.y, m_q.z };
					while (!m_readingPromises.empty())
  					{
						auto& readPromise = m_readingPromises.front();
						readPromise.set_value(out);
						m_readingPromises.pop();
						// note it is safe for promise to go out of scope here -
						// state is shared and active while associated future is live
					}
					m_bSignalRequestReading = false;
				}
			}

			std::future<Quaternion> Madgwick::ReadFusedSensorDataAsync()
			{
				// possibly called from multiple threads. protect the shared promise queue
				const std::lock_guard<std::mutex> lock(m_sensorFusionLock);

				m_readingPromises.push(std::promise<Quaternion>());
				m_bSignalRequestReading.store(true); // signal read request
				return m_readingPromises.back().get_future();
			}

			//---------------------------------------------------------------------------------------------------
			// Fast inverse square-root
			// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
			/*
			float Madgwick::invSqrt(float x) {
				float halfx = 0.5f * x;
				float y = x;
				long i = *(long*)&y;
				i = 0x5f3759df - (i>>1);
				//i = 0xdf59375f - (i>>1);
				y = *(float*)&i;
				y = y * (1.5f - (halfx * y * y));
				return y;
			}
			Replaced with this more efficient implementation: */

			float Madgwick::invSqrt(float x) {
				uint32_t i = 0x5F1F1412 - (*(uint32_t*)&x >> 1);
				float tmp = *(float*)&i;
				return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
			}
		}
	}
}