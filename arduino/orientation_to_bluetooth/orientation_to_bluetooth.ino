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
#include <Adafruit_NeoPixel.h>

#define OK 0x49D4

#define RIGHT_LSM9DS0_XM  0x1E 
#define RIGHT_LSM9DS0_G   0x6A 
#define LEFT_LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LEFT_LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW

#define RIGHT_LEDS_PIN 5
#define LEFT_LEDS_PIN 6

LSM9DS0 right_imu(MODE_I2C, RIGHT_LSM9DS0_G, RIGHT_LSM9DS0_XM);
LSM9DS0 left_imu(MODE_I2C, LEFT_LSM9DS0_G, LEFT_LSM9DS0_XM);

imu_data right_data, left_data;

float declination = 1.37; // declination at Saint Martin D'Hères, France is 1°22' E ± 0°20' on 16/05/2015

Adafruit_NeoPixel right_leds = Adafruit_NeoPixel(3, RIGHT_LEDS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel left_leds = Adafruit_NeoPixel(3, LEFT_LEDS_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200); // Start serial at 115200 bps
            
    // begin() returns a 16-bit value which includes both the gyro 
    // and accelerometers WHO_AM_I response. Check this to
    // make sure communication was successful.
    right_data.status = right_imu.begin();

    if (right_data.status != OK)
        Serial.println("Error connecting to right LSM9DS0");

    left_data.status = left_imu.begin();

    if (left_data.status != OK)
        Serial.println("Error connecting to left LSM9DS0");
        
    imu_setup(&right_imu, &right_data);
    imu_setup(&left_imu, &left_data);
    
    right_leds.begin();
    left_leds.begin();

    for(uint16_t i = 0; i < right_leds.numPixels(); i++) {
      right_leds.setPixelColor(i, 0, 0, 0);
  }

      for(uint16_t i = 0; i < left_leds.numPixels(); i++) {
      left_leds.setPixelColor(i, 0, 0, 0);
  }
    
    right_leds.show();
    left_leds.show();
}

void loop()
{
    if (Serial.available())
    {
        char readByte = Serial.read();
        if (readByte == 'r')
            colorWipe(&right_leds, right_leds.Color(0, 255, 0));
        else if (readByte == 'l')
            colorWipe(&left_leds, left_leds.Color(0, 255, 0));
        else
            Serial.println("Invalid byte received.");
    }

    // right arm imu_read
    if (right_data.status == OK)
    {
        imu_read(&right_imu, &right_data);
        MadgwickQuaternionUpdate(&right_data);
        quaternions_to_tait_bryan(&right_data);    
        Serial.print("r,"); // right
        imu_send(&right_data);
    }

    // left arm IMU
    if (left_data.status == OK)
    {
        imu_read(&left_imu, &left_data);
        MadgwickQuaternionUpdate(&left_data);
        quaternions_to_tait_bryan(&left_data);
        Serial.print("l,"); // left
        imu_send(&left_data);
    }
    
    delay(25); // updates at ~40 Hz (1/40 = 25 ms)
}

// Fill the dots one after the other with a color
void colorWipe(Adafruit_NeoPixel *strip, uint32_t c) {
  

  for(uint16_t i = 0; i < strip->numPixels(); i++) {
      strip->setPixelColor(i, c);
      strip->show();
      delay(666);
  }

  delay(1000);

  for(int16_t i = strip->numPixels() - 1; i >= 0; i--) {
      delay(666);
      strip->setPixelColor(i, 0, 0, 0); // turns off
      strip->show();
    }
}
