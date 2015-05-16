#include <Arduino.h>
#include <SFE_LSM9DS0.h>
#include "data_fusion.h"

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
#define GyroMeasError PI * (40.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 3 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)      // gyroscope measurement drift in rad/s/s (shown as 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense; 
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy. 
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

void imu_setup(LSM9DS0 *imu, imu_data *data)
{
    // Set data output ranges; choose lowest ranges for maximum resolution
    imu->setAccelScale(imu->A_SCALE_2G);
    imu->setGyroScale(imu->G_SCALE_245DPS); 
    imu->setMagScale(imu->M_SCALE_2GS);
  
    // Set output data rates  
    imu->setAccelODR(imu->A_ODR_200); // Set accelerometer update rate at 100 Hz
    // Accelerometer anti-aliasing filter rate can be 50, 194, 362, or 763 Hz
    // Anti-aliasing acts like a low-pass filter allowing oversampling of accelerometer and rejection of high-frequency spurious noise.
    // Strategy here is to effectively oversample accelerometer at 100 Hz and use a 50 Hz anti-aliasing (low-pass) filter frequency
    // to get a smooth ~150 Hz filter update rate
    imu->setAccelABW(imu->A_ABW_50); // Choose lowest filter setting for low noise
    imu->setGyroODR(imu->G_ODR_190_BW_125);  // Set gyro update rate to 190 Hz with the smallest bandwidth for low noise
    imu->setMagODR(imu->M_ODR_125); // Set magnetometer to update every 80 ms
    
    // Use the FIFO mode to average accelerometer and gyro readings to calculate the biases, which can then be removed from
    // all subsequent measurements.
    imu->calLSM9DS0(data->gbias, data->abias);
}

void imu_read(LSM9DS0 *imu, imu_data *data)
{
    imu->readGyro(); // Read raw gyro data
    data->gx = imu->calcGyro(imu->gx) - data->gbias[0]; // Convert to degrees per seconds, remove gyro biases
    data->gy = imu->calcGyro(imu->gy) - data->gbias[1];
    data->gz = imu->calcGyro(imu->gz) - data->gbias[2];

    imu->readAccel(); // Read raw accelerometer data
    data->ax = imu->calcAccel(imu->ax) - data->abias[0]; // Convert to g's, remove accelerometer biases
    data->ay = imu->calcAccel(imu->ay) - data->abias[1];
    data->az = imu->calcAccel(imu->az) - data->abias[2];

    imu->readMag();           // Read raw magnetometer data
    data->mx = imu->calcMag(imu->mx);     // Convert to Gauss and correct for calibration
    data->my = imu->calcMag(imu->my);
    data->mz = imu->calcMag(imu->mz);

    data->now = micros();
    data->deltat = ((data->now - data->last_update)/1000000.0f); // set integration time by time elapsed since last filter update
    data->last_update = data->now;
}

void imu_send(imu_data *data)
{
    Serial.print(data->roll, 0);
    Serial.print(",");
    Serial.print(data->pitch, 0);
    Serial.print(",");
    Serial.print(data->yaw, 0);
    Serial.print(",");
}

void MadgwickQuaternionUpdate(imu_data *imu)
{
    float q1 = imu->q[0], q2 = imu->q[1], q3 = imu->q[2], q4 = imu->q[3];   // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;
    
    // Sensors x- and y-axes are aligned but magnetometer z-axis (+ down) is opposite to z-axis (+ up) of accelerometer and gyro!
    // This is ok by aircraft orientation standards! 
    float ax = imu->ax, ay = imu->ay, az = imu->az;
    float gx = (imu->gx)*PI/180.0f, gy = (imu->gy)*PI/180.0f, gz = (imu->gz)*PI/180.0f; // gyro rate as rad/s
    float mx = imu->mx, my = imu->my, mz = imu->mz;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * imu->deltat;
    q2 += qDot2 * imu->deltat;
    q3 += qDot3 * imu->deltat;
    q4 += qDot4 * imu->deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    imu->q[0] = q1 * norm;
    imu->q[1] = q2 * norm;
    imu->q[2] = q3 * norm;
    imu->q[3] = q4 * norm;

}

void quaternions_to_tait_bryan(imu_data *imu)
{
    imu->yaw   = atan2(2.0f * (imu->q[1] * imu->q[2] + imu->q[0] * imu->q[3]), imu->q[0] * imu->q[0] + imu->q[1] * imu->q[1] - imu->q[2] * imu->q[2] - imu->q[3] * imu->q[3]);   
    imu->pitch = -asin(2.0f * (imu->q[1] * imu->q[3] - imu->q[0] * imu->q[2]));
    imu->roll  = atan2(2.0f * (imu->q[0] * imu->q[1] + imu->q[2] * imu->q[3]), imu->q[0] * imu->q[0] - imu->q[1] * imu->q[1] - imu->q[2] * imu->q[2] + imu->q[3] * imu->q[3]);
    imu->pitch *= 180.0f / PI;
    imu->yaw   *= 180.0f / PI; 
    imu->yaw   -= declination;
    imu->roll  *= 180.0f / PI;
}
