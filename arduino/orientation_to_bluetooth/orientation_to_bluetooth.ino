/*
Tricot Malin

This code was heavily inspired by:
 
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

#define RIGHT_LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define RIGHT_LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
#define LEFT_LSM9DS0_XM  0x1E
#define LEFT_LSM9DS0_G   0x6A

LSM9DS0 right_imu(MODE_I2C, RIGHT_LSM9DS0_G, RIGHT_LSM9DS0_XM);
LSM9DS0 left_imu(MODE_I2C, LEFT_LSM9DS0_G, LEFT_LSM9DS0_XM);

imu_data right_data, left_data;

float declination = 1.37; // declination at Saint Martin D'Hères, France is 1°22' E ± 0°20' on 16/05/2015

void setup()
{
    Serial.begin(115200); // Start serial at 115200 bps
            
    // begin() returns a 16-bit value which includes both the gyro 
    // and accelerometers WHO_AM_I response. Check this to
    // make sure communication was successful.
    uint32_t status = right_imu.begin();

    if (status != 0x49D4)
        Serial.println("Error connecting to right LSM9DS0");

    status = left_imu.begin();

    if (status != 0x49D4)
        Serial.println("Error connecting to left LSM9DS0");
        
    imu_setup(&right_imu, &right_data);
    imu_setup(&left_imu, &left_data);

}

void loop()
{
    // right arm IMU
    imu_read(&right_imu, &right_data);
    MadgwickQuaternionUpdate(&right_data);
    quaternions_to_tait_bryan(&right_data);    
    Serial.print("r#"); // right
    imu_send(&right_data);

    // left arm IMU
    imu_read(&left_imu, &left_data);
    MadgwickQuaternionUpdate(&left_data);
    quaternions_to_tait_bryan(&left_data);
    Serial.print("l#"); // left
    imu_send(&left_data);
    
    delay(25); // updates at ~40 Hz (1/40 = 25 ms)
}
