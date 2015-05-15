#ifndef DATA_FUSION_H
#define DATA_FUSION_H

extern float q[4]; // vector to hold quaternion
extern float pitch, yaw, roll, heading;
extern float declination;
extern float deltat;
extern float eInt[3]; // vector to hold integral error for Mahony method

/*
Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
(see http://www.x-io.co.uk/category/open-source/ for examples and more details)
which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
*/
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

/*
Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
measured ones. 
*/
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

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
void quaternions_to_tait_bryan();

// Another fun function that does calculations based on the
// acclerometer data. This function will print your LSM9DS0's
// orientation -- it's roll and pitch angles.
void printOrientation(float x, float y, float z);

// Here's a fun function to calculate your heading, using Earth's
// magnetic field.
// It only works if the sensor is flat (z-axis normal to Earth).
// Additionally, you may need to add or subtract a declination
// angle to get the heading normalized to your location.
// See: http://www.ngdc.noaa.gov/geomag/declination.shtml
void printHeading(float hx, float hy);

#endif // DATA_FUSION_H
