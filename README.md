# tricot-malin
Tricot Malin (smart sweater in French) is a smart t-shirt that makes it possible to interact with the digital environment that surround us through the use of simple gestures.

## Materials

* **1 JeeNode v6:** microcontroller running at 3.3V
* **2 LSM9DS0:** inertial measurement units
* **2 RedBearLab BLE Mini:** Bluetooth modules
* **2 strips of 3 LEDs of AdaFruit's NeoPixels:** luminous feedback
* **1 buzzer:** sonorous feedback
* **Jumper wires (a lot!)**: to connect everything together

## Requirements

  * [Python 2.7](https://www.python.org/downloads/)
  * [Arduino IDE](http://www.arduino.cc/en/Main/Software)
  * [Sparkun LSM9DS0 Arduino Library](https://github.com/sparkfun/SparkFun_LSM9DS0_Arduino_Library)
  * [Adafruit NeoPixel Arduino Library](https://github.com/adafruit/Adafruit_NeoPixel)
  * [BLE Mini HCI Library](https://github.com/RedBearLab/BLE_HCI)

## Instructions

* Upload the [HCI UART slave firmware](https://github.com/RedBearLab/BLE_HCI/blob/master/cc2540_hci_fw/HCI_UART_115200bps_20130429.bin.zip) to the BLE Mini which will be connected to the JeeNode.
* Connect the components to the JeeNode according to the schematic.
* Upload the code `arduino/orientation_to_bluetooth/orientation_to_bluetooth.ino` to the JeeNode:
  * Disconnect BLE Mini from TX and RX pins
  * Open the code in the Arduino IDE
  * Select Arduino Uno as board and the appropriate serial port
  * Press upload
  * Reconnect BLE Mini to TX and RX pins

* Connect the BLE Mini containing the [HCI central firmware](https://github.com/RedBearLab/BLE_HCI/blob/master/cc2540_hci_fw/HCI_USBCDC_115200_20130429.bin.zip) to your computer with a Micro USB cable. 
* Set the global variable `TX.Port` in `bluetooth/bluetooth_central.py` to the approriate port to which the BLE Mini is connected.
* Run the Python code `bluetooth_central.py`.
  * Press `n` to enable notifications and then `d` to search for BLE modules nearby.
  * After the discovery, press `e` to establish the conneion to the BLE module connected to the JeeNode.
  * When the connection is successful, press `n` again to start the communication.
  * To end the execution, press `q` and then close the visualization windows.