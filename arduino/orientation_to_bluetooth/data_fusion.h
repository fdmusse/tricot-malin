#ifndef DATA_FUSION_H
#define DATA_FUSION_H

class LSM9DS0; // forward declaration

struct imu_data
{
    float abias[3] = {0, 0, 0}, gbias[3] = {0, 0, 0};
    float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
    float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; // vector to hold quaternion
    float pitch, yaw, roll;
    uint32_t last_update = 0; // used to calculate integration interval
    uint32_t now = 0; // used to calculate integration interval
    float deltat = 0.0f; // integration interval for both filter schemes
};

extern float declination;

void imu_setup(LSM9DS0 *imu, imu_data *data);

void imu_read(LSM9DS0 *imu, imu_data *data);

void imu_send(imu_data *data);

/*
Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
(see http://www.x-io.co.uk/category/open-source/ for examples and more details)
which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
*/
void MadgwickQuaternionUpdate(imu_data *imu);

/*
Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
In this coordinate system, the positive z-axis is down toward Earth. 
Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination), 
looking down on the sensor positive yaw is counterclockwise.
Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
Tait-Bryan angles as well as Euler angles are non-commutative; that is, to get the correct orientation the rotations must be
applied in the correct order which for this configuration is yaw, pitch, and then roll.
For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
*/
void quaternions_to_tait_bryan(imu_data *imu);

#endif // DATA_FUSION_H
