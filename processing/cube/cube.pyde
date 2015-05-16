add_library('serial')
#import serial

portName = Serial.list()[2] #may need to be changed to match your port
myPort = Serial(this, portName, 38400) #  115200 bps is the same baud rate as Arduino


pitch = 0.0
roll = 0.0
yaw = 0.0
def setup():
    size(640, 360, P3D)
    noStroke() 
    colorMode(RGB, 1)
def draw():
    global pitch
    global roll
    global yaw
    background(0.5)
  
    pushMatrix()
    translate(width/2, height/2, -30)
  
    if myPort.available()>0:
        yaw = float(myPort.readStringUntil('\n'))
        print yaw + "\n"
        pitch = float(myPort.readStringUntil('\n'))
        print pitch + "\n"
        roll = float(myPort.readStringUntil('\n'))
        print roll + "\n"
   
    rotateX(pitch)
    rotateY(-roll) 
    
    scale(90)
    beginShape(QUADS)
    
    fill(0, 1, 1) 
    vertex(-1,  1,  1)
    fill(1, 1, 1) 
    vertex( 1,  1,  1)
    fill(1, 0, 1) 
    vertex( 1, -1,  1)
    fill(0, 0, 1) 
    vertex(-1, -1,  1)
    
    fill(1, 1, 1) 
    vertex( 1,  1,  1)
    fill(1, 1, 0) 
    vertex( 1,  1, -1)
    fill(1, 0, 0) 
    vertex( 1, -1, -1)
    fill(1, 0, 1) 
    vertex( 1, -1,  1)
    
    fill(1, 1, 0) 
    vertex( 1,  1, -1)
    fill(0, 1, 0) 
    vertex(-1,  1, -1)
    fill(0, 0, 0) 
    vertex(-1, -1, -1)
    fill(1, 0, 0) 
    vertex( 1, -1, -1)
    
    fill(0, 1, 0) 
    vertex(-1,  1, -1)
    fill(0, 1, 1) 
    vertex(-1,  1,  1)
    fill(0, 0, 1) 
    vertex(-1, -1,  1)
    fill(0, 0, 0) 
    vertex(-1, -1, -1)
    
    fill(0, 1, 0) 
    vertex(-1,  1, -1)
    fill(1, 1, 0) 
    vertex( 1,  1, -1)
    fill(1, 1, 1) 
    vertex( 1,  1,  1)
    fill(0, 1, 1) 
    vertex(-1,  1,  1)
    
    fill(0, 0, 0) 
    vertex(-1, -1, -1)
    fill(1, 0, 0) 
    vertex( 1, -1, -1)
    fill(1, 0, 1) 
    vertex( 1, -1,  1)
    fill(0, 0, 1) 
    vertex(-1, -1,  1)
    
    endShape()
    
    popMatrix() 

