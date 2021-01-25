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

			Madgwick::Madgwick(float sampleFreq, float betaDef) :
				beta(betaDef),
				m_fSampleFreqHz(sampleFreq),
				m_q{1.0f, 0.0f, 0.0f, 0.0f},
				m_qOut{},
				m_bSignalRequestReading(false)
			{
			}

			Madgwick::~Madgwick()
			{
			}

			bool Madgwick::IsHardwareImplementation()
			{
				return false;
			}

			void Madgwick::Update(const Axis3DSensorData& gyroData, const Axis3DSensorData& accelData, const Axis3DSensorData& magData)
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

				// Rate of change of quaternion from gyroscope
				qDot1 = 0.5f * (-m_q.x * gyroData.x - m_q.y * gyroData.y - m_q.z * gyroData.z);
				qDot2 = 0.5f * (m_q.w * gyroData.x + m_q.y * gyroData.z - m_q.z * gyroData.y);
				qDot3 = 0.5f * (m_q.w * gyroData.y - m_q.x * gyroData.z + m_q.z * gyroData.x);
				qDot4 = 0.5f * (m_q.w * gyroData.z + m_q.x * gyroData.y - m_q.y * gyroData.x);

				// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
				if(!((accelData.x == 0.0f) && (accelData.y == 0.0f) && (accelData.z == 0.0f))) {

					// Normalise accelerometer measurement
					recipNorm = invSqrt(accelData.x * accelData.x + accelData.y * accelData.y + accelData.z * accelData.z);
					accelData.x *= recipNorm;
					accelData.y *= recipNorm;
					accelData.z *= recipNorm;   

					// Normalise magnetometer measurement
					recipNorm = invSqrt(magData.x * magData.x + magData.y * magData.y + magData.z * magData.z);
					mx *= recipNorm;
					my *= recipNorm;
					mz *= recipNorm;

					// Auxiliary variables to avoid repeated arithmetic
					_2q0mx = 2.0f * m_q.w * mx;
					_2q0my = 2.0f * m_q.w * my;
					_2q0mz = 2.0f * m_q.w * mz;
					_2q1mx = 2.0f * m_q.x * mx;
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
					hx = mx * q0q0 - _2q0my * m_q.z + _2q0mz * m_q.y + mx * q1q1 + _2q1 * my * m_q.y + _2q1 * mz * m_q.z - mx * q2q2 - mx * q3q3;
					hy = _2q0mx * m_q.z + my * q0q0 - _2q0mz * m_q.x + _2q1mx * m_q.y - my * q1q1 + my * q2q2 + _2q2 * mz * m_q.z - my * q3q3;
					_2bx = sqrt(hx * hx + hy * hy);
					_2bz = -_2q0mx * m_q.y + _2q0my * m_q.x + mz * q0q0 + _2q1mx * m_q.z - mz * q1q1 + _2q2 * my * m_q.z - mz * q2q2 + mz * q3q3;
					_4bx = 2.0f * _2bx;
					_4bz = 2.0f * _2bz;

					// Gradient decent algorithm corrective step
					s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * m_q.y * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * m_q.z + _2bz * m_q.x) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * m_q.y * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
					s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * m_q.x * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * m_q.z * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * m_q.y + _2bz * m_q.w) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * m_q.z - _4bz * m_q.x) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
					s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * m_q.y * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * m_q.y - _2bz * m_q.w) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * m_q.x + _2bz * m_q.z) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * m_q.w - _4bz * m_q.y) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
					s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * m_q.z + _2bz * m_q.x) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * m_q.w + _2bz * m_q.y) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * m_q.x * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
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
				m_q.w += qDot1 * (1.0f / m_fSampleFreqHz);
				m_q.x += qDot2 * (1.0f / m_fSampleFreqHz);
				m_q.y += qDot3 * (1.0f / m_fSampleFreqHz);
				m_q.z += qDot4 * (1.0f / m_fSampleFreqHz);

				// Normalise quaternion
				recipNorm = invSqrt(m_q.w * m_q.w + m_q.x * m_q.x + m_q.y * m_q.y + m_q.z * m_q.z);
				m_q.w *= recipNorm;
				m_q.x *= recipNorm;
				m_q.y *= recipNorm;
				m_q.z *= recipNorm;
			}

			//---------------------------------------------------------------------------------------------------
			// IMU algorithm update

			void UpdateIMU(Axis3DSensorData gyroData, Axis3DSensorData accelData)
			{
				float recipNorm;
				float s0, s1, s2, s3;
				float qDot1, qDot2, qDot3, qDot4;
				float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

				// Rate of change of quaternion from gyroscope
				qDot1 = 0.5f * (-m_q.x * gx - m_q.y * gy - m_q.z * gz);
				qDot2 = 0.5f * (m_q.w * gx + m_q.y * gz - m_q.z * gy);
				qDot3 = 0.5f * (m_q.w * gy - m_q.x * gz + m_q.z * gx);
				qDot4 = 0.5f * (m_q.w * gz + m_q.x * gy - m_q.y * gx);

				// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
				if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

					// Normalise accelerometer measurement
					recipNorm = invSqrt(ax * ax + ay * ay + az * az);
					ax *= recipNorm;
					ay *= recipNorm;
					az *= recipNorm;   

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
					s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
					s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * m_q.x - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
					s2 = 4.0f * q0q0 * m_q.y + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
					s3 = 4.0f * q1q1 * m_q.z - _2q1 * ax + 4.0f * q2q2 * m_q.z - _2q2 * ay;
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
				m_q.w += qDot1 * (1.0f / m_fSampleFreqHz);
				m_q.x += qDot2 * (1.0f / m_fSampleFreqHz);
				m_q.y += qDot3 * (1.0f / m_fSampleFreqHz);
				m_q.z += qDot4 * (1.0f / m_fSampleFreqHz);

				// Normalise quaternion
				recipNorm = invSqrt(m_q.w * m_q.w + m_q.x * m_q.x + m_q.y * m_q.y + m_q.z * m_q.z);
				m_q.w *= recipNorm;
				m_q.x *= recipNorm;
				m_q.y *= recipNorm;
				m_q.z *= recipNorm;

				if (m_bSignalRequestReading) {
					m_qOut.q0 = m_q.w;
					m_qOut.q1 = m_q.x;
					m_qOut.q2 = m_q.y;
					m_qOut.q3 = m_q.z;
					m_readingPromise.set_value(m_qOut);
					m_bSignalRequestReading = false;
				}
			}

			std::future<Quarternion> Madgwick::ReadFusedSensorDataAsync()
			{
				if (m_bSignalRequestReading) {
					throw std::runtime_error("signal reading already requested");
				}

				m_readingPromise = std::promise<Quarternion>();
				m_bSignalRequestReading = true;
				return m_readingPromise.get_future();
			}

			//---------------------------------------------------------------------------------------------------
			// Fast inverse square-root
			// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

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
		}
	}
}