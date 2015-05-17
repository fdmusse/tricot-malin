# tricot-malin

Tricot Malin (smart sweater in French) is a smart t-shirt that makes it possible to interact with the digital environment that surround us through the use of simple gestures.

## Materials

* *1 [JeeNode v6](http://jeelabs.net/projects/hardware/wiki/JeeNode):* microcontroller running at 3.3V
* *2 [Sparkfun LSM9DS0 breakout board](https://www.sparkfun.com/products/12636):* inertial measurement units, one for each arm
* *2 [RedBearLab BLE Mini](http://redbearlab.com/blemini/):* Bluetooth modules
* *2 RGB LED strips of [AdaFruit NeoPixel](http://www.adafruit.com/product/1460):* luminous feedback, one for each arm
* *1 buzzer:* sonorous feedback
* *Jumper wires (a lot!)*: to connect everything together
* *FTDI USB to serial board:* to upload code to the JeeNode

## Requirements

  * [Python 2.7](https://www.python.org/downloads/)
  * [Arduino IDE](http://www.arduino.cc/en/Main/Software)
  * [Sparkun LSM9DS0 Arduino Library](https://github.com/sparkfun/SparkFun_LSM9DS0_Arduino_Library)
  * [Adafruit NeoPixel Arduino Library](https://github.com/adafruit/Adafruit_NeoPixel)
  * [BLE Mini HCI Library](https://github.com/RedBearLab/BLE_HCI)

## Instructions

1. Upload the [HCI UART slave firmware](https://github.com/RedBearLab/BLE_HCI/blob/master/cc2540_hci_fw/HCI_UART_115200bps_20130429.bin.zip) to the BLE Mini which will be connected to the JeeNode.
2. Connect the components to the JeeNode according to the schematic using the jumper wires.
3. Upload the code `arduino/orientation_to_bluetooth/orientation_to_bluetooth.ino` to the JeeNode:
 * Connect the JeeNode to your computer with a FTDI USB to serial board, such as the [USB BUB II](http://moderndevice.com/product/usb-bub-ii/). 
  * Open the code in the Arduino IDE.
  * Select `Arduino Uno` as board and the appropriate serial port.
  * Disconnect BLE Mini from `TX` and `RX` pins.
  * Press upload.
  * Reconnect BLE Mini to `TX` and `RX` pins.

4. Connect the BLE Mini containing the [HCI central firmware](https://github.com/RedBearLab/BLE_HCI/blob/master/cc2540_hci_fw/HCI_USBCDC_115200_20130429.bin.zip) to your computer with a Micro USB cable. 
5. Set the global variable `TX.Port` in `bluetooth/bluetooth_central.py` to the approriate port to which the BLE Mini is connected.
6. Run the Python code `bluetooth_central.py`:
  * Press `n` to enable notifications and then `d` to search for BLE modules nearby.
  * After the discovery, press `e` to establish the connection to the BLE module connected to the JeeNode.
  * When the connection is successful, press `n` again to start the communication.
  * To end the execution, press `q` and then close any visualization window.

##  Usage

* Turning an arm by 180° will trigger the corresponding action.
* When a gesture is detected, the LEDs of the arm will shortly turn on.
* When the lights turn off, a new gesture is ready to be detected. 
* The gesture of each arm has its own an independent associated action.

## Documentation

All the project's documentation is available at our [Wiki](https://github.com/fdmusse/tricot-malin/wiki).