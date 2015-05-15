import serial


bluetoothSerial = serial.Serial( "/dev/tty.Bluetooth-Incoming-Port", baudrate=38400 ) # may need to be changed to match your port


pitch = 0
roll = 0

 
def main():

    #size(640, 360, P3D); 
    #noStroke(); 
    #colorMode(RGB, 1); 
    looping = True;

    while looping:
        #background(0.5);
  
        #pushMatrix(); 
        #translate(width/2, height/2, -30); 
        bluetoothSerial.flushInput()
        serialValue = bluetoothSerial.readline().strip()
        print serialValue
       
main()