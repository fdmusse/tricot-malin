import processing.serial.*;


String val;     // Data received from the serial port

String portName = Serial.list()[2]; //may need to be changed to match your port

// Create object from Serial class
Serial myPort = new Serial(this, portName, 38400); //  115200 bps is the same baud rate as Arduino 


float pitch = 0;
float roll = 0;

 
void setup()  { 
  size(640, 360, P3D); 
  noStroke(); 
  colorMode(RGB, 1); 
} 
 
void draw()  { 
  background(0.5);
  
  pushMatrix(); 
  translate(width/2, height/2, -30); 
  
  if (myPort.available()>0) {
    val = myPort.readStringUntil('\n');
    println(val);
    if( val != null && !val.isEmpty()){
        int indexComma = val.indexOf(',');
    //println(indexComma);
    if(indexComma > 0){
      String pitchStr = val.substring(0,indexComma);
      //println(pitchStr);
      pitch = Float.valueOf(pitchStr).floatValue();
       //println(pitch);
       
       String rollStr = val.substring(indexComma+1, val.length());
       roll = Float.valueOf(rollStr).floatValue();
        // println(rollStr);
    }
    
    }
    
  }
 
  
  rotateX(pitch); 
  rotateY(-roll); 
  
  scale(90);
  beginShape(QUADS);

  fill(0, 1, 1); vertex(-1,  1,  1);
  fill(1, 1, 1); vertex( 1,  1,  1);
  fill(1, 0, 1); vertex( 1, -1,  1);
  fill(0, 0, 1); vertex(-1, -1,  1);

  fill(1, 1, 1); vertex( 1,  1,  1);
  fill(1, 1, 0); vertex( 1,  1, -1);
  fill(1, 0, 0); vertex( 1, -1, -1);
  fill(1, 0, 1); vertex( 1, -1,  1);

  fill(1, 1, 0); vertex( 1,  1, -1);
  fill(0, 1, 0); vertex(-1,  1, -1);
  fill(0, 0, 0); vertex(-1, -1, -1);
  fill(1, 0, 0); vertex( 1, -1, -1);

  fill(0, 1, 0); vertex(-1,  1, -1);
  fill(0, 1, 1); vertex(-1,  1,  1);
  fill(0, 0, 1); vertex(-1, -1,  1);
  fill(0, 0, 0); vertex(-1, -1, -1);

  fill(0, 1, 0); vertex(-1,  1, -1);
  fill(1, 1, 0); vertex( 1,  1, -1);
  fill(1, 1, 1); vertex( 1,  1,  1);
  fill(0, 1, 1); vertex(-1,  1,  1);

  fill(0, 0, 0); vertex(-1, -1, -1);
  fill(1, 0, 0); vertex( 1, -1, -1);
  fill(1, 0, 1); vertex( 1, -1,  1);
  fill(0, 0, 1); vertex(-1, -1,  1);

  endShape();
  
  popMatrix(); 
  
} 
