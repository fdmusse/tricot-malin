from visual import *
import string
import math

deg2rad = 3.141592/180.0

class Visualization:
    def __init__(self, windowTitle):
        # Main scene
        self.scene = display(title=windowTitle)
        self.scene.range = (1.2,1.2,1.2)
        #self.scene.forward = (0,-1,-0.25)
        self.scene.forward = (1,0,-0.25)
        self.scene.up=(0,0,1)

        # Second scene (Roll, Pitch, Yaw)
        self.scene2 = display(title=windowTitle, x=0, y=0, width=500, height=200,center=(0,0,0), background=(0,0,0))
        self.scene2.range=(1,1,1)
        self.scene.width=500
        self.scene.y=200

        self.scene2.select()
        #Roll, Pitch, Yaw
        self.cil_roll = cylinder(pos=(-0.4,0,0),axis=(0.2,0,0),radius=0.01,color=color.red)
        self.cil_roll2 = cylinder(pos=(-0.4,0,0),axis=(-0.2,0,0),radius=0.01,color=color.red)
        self.cil_pitch = cylinder(pos=(0.1,0,0),axis=(0.2,0,0),radius=0.01,color=color.green)
        self.cil_pitch2 = cylinder(pos=(0.1,0,0),axis=(-0.2,0,0),radius=0.01,color=color.green)
        #cil_course = cylinder(pos=(0.6,0,0),axis=(0.2,0,0),radius=0.01,color=color.blue)
        #cil_course2 = cylinder(pos=(0.6,0,0),axis=(-0.2,0,0),radius=0.01,color=color.blue)
        self.arrow_course = arrow(pos=(0.6,0,0),color=color.cyan,axis=(-0.2,0,0), shaftwidth=0.02, fixedwidth=1)

        #Roll,Pitch,Yaw labels
        label(pos=(-0.4,0.3,0),text="Roll",box=0,opacity=0)
        label(pos=(0.1,0.3,0),text="Pitch",box=0,opacity=0)
        label(pos=(0.55,0.3,0),text="Yaw",box=0,opacity=0)

        label(pos=(0.6,0.22,0),text="N",box=0,opacity=0,color=color.yellow)
        label(pos=(0.6,-0.22,0),text="S",box=0,opacity=0,color=color.yellow)
        label(pos=(0.38,0,0),text="W",box=0,opacity=0,color=color.yellow)
        label(pos=(0.82,0,0),text="E",box=0,opacity=0,color=color.yellow)
        label(pos=(0.75,0.15,0),height=7,text="NE",box=0,color=color.yellow)
        label(pos=(0.45,0.15,0),height=7,text="NW",box=0,color=color.yellow)
        label(pos=(0.75,-0.15,0),height=7,text="SE",box=0,color=color.yellow)
        label(pos=(0.45,-0.15,0),height=7,text="SW",box=0,color=color.yellow)

        self.L1 = label(pos=(-0.4,0.22,0),text="-",box=0,opacity=0)
        self.L2 = label(pos=(0.1,0.22,0),text="-",box=0,opacity=0)
        self.L3 = label(pos=(0.7,0.3,0),text="-",box=0,opacity=0)

        # Main scene objects
        self.scene.select()
        # Reference axis (x,y,z)
        arrow(color=color.green,axis=(1,0,0), shaftwidth=0.02, fixedwidth=1)
        arrow(color=color.green,axis=(0,-1,0), shaftwidth=0.02 , fixedwidth=1)
        arrow(color=color.green,axis=(0,0,-1), shaftwidth=0.02, fixedwidth=1)
        # labels
        label(pos=(0,0,0.8),text="9DOF Razor IMU test",box=0,opacity=0)
        label(pos=(1,0,0),text="X",box=0,opacity=0)
        label(pos=(0,-1,0),text="Y",box=0,opacity=0)
        label(pos=(0,0,-1),text="Z",box=0,opacity=0)
        # IMU object
        self.platform = box(length=1, height=0.05, width=1, color=color.red)
        self.p_line = box(length=1,height=0.08,width=0.1,color=color.yellow)
        self.plat_arrow = arrow(color=color.green,axis=(1,0,0), shaftwidth=0.06, fixedwidth=1)

        self.roll=0
        self.pitch=0
        self.yaw=0
    
    def update(self, words):      
        #line = line.replace("!ANG:","")   # Delete "!ANG:"
        #print line
        #words = string.split(line,",")    # Fields split
        if len(words) > 2:
            try:
                roll = float(words[0])*deg2rad
                pitch = float(words[1])*deg2rad
                yaw = float(words[2])*deg2rad
            except:
                print "Invalid line"

            axis=(cos(pitch)*cos(yaw),-cos(pitch)*sin(yaw),sin(pitch)) 
            up=(sin(roll)*sin(yaw)+cos(roll)*sin(pitch)*cos(yaw),sin(roll)*cos(yaw)-cos(roll)*sin(pitch)*sin(yaw),-cos(roll)*cos(pitch))
            self.platform.axis=axis
            self.platform.up=up
            self.platform.length=1.0
            self.platform.width=0.65
            self.plat_arrow.axis=axis
            self.plat_arrow.up=up
            self.plat_arrow.length=0.8
            self.p_line.axis=axis
            self.p_line.up=up
            self.cil_roll.axis=(0.2*cos(roll),0.2*sin(roll),0)
            self.cil_roll2.axis=(-0.2*cos(roll),-0.2*sin(roll),0)
            self.cil_pitch.axis=(0.2*cos(pitch),0.2*sin(pitch),0)
            self.cil_pitch2.axis=(-0.2*cos(pitch),-0.2*sin(pitch),0)
            self.arrow_course.axis=(0.2*sin(yaw),0.2*cos(yaw),0)
            self.L1.text = str(float(words[0]))
            self.L2.text = str(float(words[1]))
            self.L3.text = str(float(words[2]))
            
if __name__ == "__main__":
    rightVisualization = Visualization("Right hand IMU")
    leftVisualization = Visualization("Left hand IMU")
