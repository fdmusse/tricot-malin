/*
Tricot Malin

This codew was heavily inspired by:
 - AHRS example from Sparkfun's LSM9DS0 library (https://github.com/sparkfun/
 LSM9DS0_Breakout/blob/master/Libraries/Arduino/examples/SparkFun_LSM9DS0_AHRS/
 SparkFun_LSM9DS0_AHRS.ino)
 - BiscuitPeripheral example from RedBearLab's BLE HCI library (https://github.
 com/RedBearLab/BLE_HCI/blob/master/arduino/libraries/BLEHCI/examples/Biscuit
 Peripheral/BiscuitPeripheral.ino)

LSM9DS0 being used in I2C mode.

*/

#include <SPI.h> /* Included for SFE_LSM9DS0 library, which requires both the 
SPI and Wire libraries even though we are only using I2C */
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include "data_fusion.h"

// Example I2C Setup

// SDO_XM and SDO_G are both pulled-up, so our addresses are:

/*
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
*/

#define LSM9DS0_XM  0x1E // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6A // Would be 0x6A if SDO_G is LOW

// Create an instance of the LSM9DS0 library called `dof` the
// parameters for this constructor are:
// [SPI or I2C Mode declaration],[gyro I2C address],[xm I2C add.]
LSM9DS0 dof(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);


/*
// Interrupt Pin Definitions 
const byte INT1XM = 3; // INT1XM tells us when accel data is ready
const byte INT2XM = 14; // INT2XM tells us when mag data is ready
const byte DRDYG  = 4; // DRDYG  tells us when gyro data is ready
*/

uint32_t lastUpdate = 0;    // used to calculate integration interval
uint32_t Now = 0;           // used to calculate integration interval

float abias[3] = {0, 0, 0}, gbias[3] = {0, 0, 0};
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 

float temperature;

// variables declared in data_fusion.h
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f}; // vector to hold quaternion
float pitch, yaw, roll, heading;
float declination = 13.8; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
float deltat = 0.0f; // integration interval for both filter schemes
float eInt[3] = {0.0f, 0.0f, 0.0f}; // vector to hold integral error for Mahony method

void setup()
{
    Serial.begin(115200); // Start serial at 115200 bps

    /*
    // Set up interrupt pins as inputs
    pinMode(INT1XM, INPUT);
    pinMode(INT2XM, INPUT);
    pinMode(DRDYG,  INPUT);
    */
            
    // begin() returns a 16-bit value which includes both the gyro 
    // and accelerometers WHO_AM_I response. Check this to
    // make sure communication was successful.
    uint32_t status = dof.begin();

    if (status != 0x49D4)
        Serial.println("Error connecting to LSM9DS0");
  
    // Set data output ranges; choose lowest ranges for maximum resolution
    dof.setAccelScale(dof.A_SCALE_2G);
    dof.setGyroScale(dof.G_SCALE_245DPS); 
    dof.setMagScale(dof.M_SCALE_2GS);
  
    // Set output data rates  
    dof.setAccelODR(dof.A_ODR_200); // Set accelerometer update rate at 100 Hz
    // Accelerometer anti-aliasing filter rate can be 50, 194, 362, or 763 Hz
    // Anti-aliasing acts like a low-pass filter allowing oversampling of accelerometer and rejection of high-frequency spurious noise.
    // Strategy here is to effectively oversample accelerometer at 100 Hz and use a 50 Hz anti-aliasing (low-pass) filter frequency
    // to get a smooth ~150 Hz filter update rate
    dof.setAccelABW(dof.A_ABW_50); // Choose lowest filter setting for low noise
    dof.setGyroODR(dof.G_ODR_190_BW_125);  // Set gyro update rate to 190 Hz with the smallest bandwidth for low noise
    dof.setMagODR(dof.M_ODR_125); // Set magnetometer to update every 80 ms
    
    // Use the FIFO mode to average accelerometer and gyro readings to calculate the biases, which can then be removed from
    // all subsequent measurements.
    dof.calLSM9DS0(gbias, abias);
}

void loop()
{
    dof.readGyro();           // Read raw gyro data
    gx = dof.calcGyro(dof.gx) - gbias[0];   // Convert to degrees per seconds, remove gyro biases
    gy = dof.calcGyro(dof.gy) - gbias[1];
    gz = dof.calcGyro(dof.gz) - gbias[2];

    dof.readAccel();         // Read raw accelerometer data
    ax = dof.calcAccel(dof.ax) - abias[0];   // Convert to g's, remove accelerometer biases
    ay = dof.calcAccel(dof.ay) - abias[1];
    az = dof.calcAccel(dof.az) - abias[2];

    dof.readMag();           // Read raw magnetometer data
    mx = dof.calcMag(dof.mx);     // Convert to Gauss and correct for calibration
    my = dof.calcMag(dof.my);
    mz = dof.calcMag(dof.mz);

    dof.readTemp();
    temperature = 21.0 + (float) dof.temperature/8.; // slope is 8 LSB per degree C, just guessing at the intercept

    Now = micros();
    deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
    lastUpdate = Now;
    
    // Sensors x- and y-axes are aligned but magnetometer z-axis (+ down) is opposite to z-axis (+ up) of accelerometer and gyro!
    // This is ok by aircraft orientation standards!  
    // Pass gyro rate as rad/s
    MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, mx, my, mz);
    //MahonyQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, mx, my, mz);

    
    // Print the heading and orientation for fun!
    //printHeading(mx, my);
    //printOrientation(ax, ay, az);

    quaternions_to_tait_bryan();

    /*
    Serial.print("ax = "); Serial.print((int)1000*ax);  
    Serial.print(" ay = "); Serial.print((int)1000*ay); 
    Serial.print(" az = "); Serial.print((int)1000*az); Serial.println(" mg");
    Serial.print("gx = "); Serial.print( gx, 2); 
    Serial.print(" gy = "); Serial.print( gy, 2); 
    Serial.print(" gz = "); Serial.print( gz, 2); Serial.println(" deg/s");
    Serial.print("mx = "); Serial.print( (int)1000*mx); 
    Serial.print(" my = "); Serial.print( (int)1000*my); 
    Serial.print(" mz = "); Serial.print( (int)1000*mz); Serial.println(" mG");

    Serial.print("temperature = "); Serial.println(temperature, 2);
    */
    
    //Serial.print("Yaw, Pitch, Roll: ");
    Serial.println(yaw, 2);
    //Serial.print(", ");
    Serial.println(pitch, 2);
    //Serial.print(", ");
    Serial.println(roll, 2);

    /*
    Serial.print("q0 = "); Serial.print(q[0]);
    Serial.print(" qx = "); Serial.print(q[1]); 
    Serial.print(" qy = "); Serial.print(q[2]); 
    Serial.print(" qz = "); Serial.println(q[3]); 
    

    Serial.print("filter rate = "); Serial.println(1.0f/deltat, 1);
    */
  
    /*
    With ODR settings of 400 Hz, 380 Hz, and 25 Hz for the accelerometer, gyro, and magnetometer, respectively,
    the filter is updating at a ~125 Hz rate using the Madgwick scheme and ~165 Hz using the Mahony scheme 
    even though the display refreshes at only 2 Hz.
    The filter update rate can be increased by reducing the rate of data reading. The optimal implementation is
    one which balances the competing rates so they are about the same; that is, the filter updates the sensor orientation
    at about the same rate the data is changing. Of course, other implementations are possible. One might consider
    updating the filter at twice the average new data rate to allow for finite filter convergence times.
    The filter update rate is determined mostly by the mathematical steps in the respective algorithms, 
    the processor speed (8 MHz for the 3.3V Pro Mini), and the sensor ODRs, especially the magnetometer ODR:
    smaller ODRs for the magnetometer produce the above rates, maximum magnetometer ODR of 100 Hz produces
    filter update rates of ~110 and ~135 Hz for the Madgwick and Mahony schemes, respectively. 
    is presumably because the magnetometer read takes longer than the gyro or accelerometer reads.
    With low ODR settings of 100 Hz, 95 Hz, and 6.25 Hz for the accelerometer, gyro, and magnetometer, respectively,
    the filter is updating at a ~150 Hz rate using the Madgwick scheme and ~200 Hz using the Mahony scheme.
    These filter update rates should be fast enough to maintain accurate platform orientation for 
    stabilization control of a fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
    produced by the on-board Digital Motion Processor of Invensense's MPU6050 6 DoF and MPU9150 9DoF sensors.
    The 3.3 V 8 MHz Pro Mini is doing pretty well!
    */
    
    //display.setCursor(0, 40); display.print("rt: "); display.print((1/deltat)); display.print(" Hz"); 
    //display.display();
     
     delay(25); // updates at 40 Hz (1/40 = 25 ms)   
}
