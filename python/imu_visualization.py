import visual
from math import cos, sin

deg2rad = 3.141592/180.0

class Visualization:
    def __init__(self, windowTitle):
        # Main scene
        self.scene = visual.display(title=windowTitle)
        self.scene.range = (1.2,1.2,1.2)
        #self.scene.forward = (0,-1,-0.25)
        self.scene.forward = (1,0,-0.25)
        self.scene.up=(0,0,1)

        # Second scene (Roll, Pitch, Yaw)
        self.scene2 = visual.display(title=windowTitle, x=0, y=0, width=500, height=200,center=(0,0,0), background=(0,0,0))
        self.scene2.range=(1,1,1)
        self.scene.width=500
        self.scene.y=200

        self.scene2.select()
        #Roll, Pitch, Yaw
        self.cil_roll = visual.cylinder(pos=(-0.4,0,0),axis=(0.2,0,0),radius=0.01,color=visual.color.red)
        self.cil_roll2 = visual.cylinder(pos=(-0.4,0,0),axis=(-0.2,0,0),radius=0.01,color=visual.color.red)
        self.cil_pitch = visual.cylinder(pos=(0.1,0,0),axis=(0.2,0,0),radius=0.01,color=visual.color.green)
        self.cil_pitch2 = visual.cylinder(pos=(0.1,0,0),axis=(-0.2,0,0),radius=0.01,color=visual.color.green)
        #cil_course = visual.cylinder(pos=(0.6,0,0),axis=(0.2,0,0),radius=0.01,color=visual.color.blue)
        #cil_course2 = visual.cylinder(pos=(0.6,0,0),axis=(-0.2,0,0),radius=0.01,color=visual.color.blue)
        self.arrow_course = visual.arrow(pos=(0.6,0,0),color=visual.color.cyan,axis=(-0.2,0,0), shaftwidth=0.02, fixedwidth=1)

        #Roll,Pitch,Yaw visual.labels
        visual.label(pos=(-0.4,0.3,0),text="Roll",box=0,opacity=0)
        visual.label(pos=(0.1,0.3,0),text="Pitch",box=0,opacity=0)
        visual.label(pos=(0.55,0.3,0),text="Yaw",box=0,opacity=0)

        visual.label(pos=(0.6,0.22,0),text="N",box=0,opacity=0,color=visual.color.yellow)
        visual.label(pos=(0.6,-0.22,0),text="S",box=0,opacity=0,color=visual.color.yellow)
        visual.label(pos=(0.38,0,0),text="W",box=0,opacity=0,color=visual.color.yellow)
        visual.label(pos=(0.82,0,0),text="E",box=0,opacity=0,color=visual.color.yellow)
        visual.label(pos=(0.75,0.15,0),height=7,text="NE",box=0,color=visual.color.yellow)
        visual.label(pos=(0.45,0.15,0),height=7,text="NW",box=0,color=visual.color.yellow)
        visual.label(pos=(0.75,-0.15,0),height=7,text="SE",box=0,color=visual.color.yellow)
        visual.label(pos=(0.45,-0.15,0),height=7,text="SW",box=0,color=visual.color.yellow)

        self.L1 = visual.label(pos=(-0.4,0.22,0),text="-",box=0,opacity=0)
        self.L2 = visual.label(pos=(0.1,0.22,0),text="-",box=0,opacity=0)
        self.L3 = visual.label(pos=(0.7,0.3,0),text="-",box=0,opacity=0)

        # Main scene objects
        self.scene.select()
        # Reference axis (x,y,z)
        visual.arrow(color=visual.color.green,axis=(1,0,0), shaftwidth=0.02, fixedwidth=1)
        visual.arrow(color=visual.color.green,axis=(0,-1,0), shaftwidth=0.02 , fixedwidth=1)
        visual.arrow(color=visual.color.green,axis=(0,0,-1), shaftwidth=0.02, fixedwidth=1)
        # visual.labels
        visual.label(pos=(0,0,0.8),text=windowTitle, box=0,opacity=0)
        visual.label(pos=(1,0,0),text="X",box=0,opacity=0)
        visual.label(pos=(0,-1,0),text="Y",box=0,opacity=0)
        visual.label(pos=(0,0,-1),text="Z",box=0,opacity=0)
        # IMU object
        self.platform = visual.box(length=1, height=0.05, width=1, color=visual.color.red)
        self.p_line = visual.box(length=1,height=0.08,width=0.1,color=visual.color.yellow)
        self.plat_arrow = visual.arrow(color=visual.color.green,axis=(1,0,0), shaftwidth=0.06, fixedwidth=1)

        self.roll=0
        self.pitch=0
        self.yaw=0
    
    def update(self, roll, pitch, yaw):      

        self.roll = roll*deg2rad
        self.pitch = pitch*deg2rad
        self.yaw = yaw*deg2rad

        axis=(cos(self.pitch)*cos(self.yaw),-cos(self.pitch)*sin(self.yaw),sin(self.pitch)) 
        up=(sin(self.roll)*sin(self.yaw)+cos(self.roll)*sin(self.pitch)*cos(self.yaw),sin(self.roll)*cos(self.yaw)-cos(self.roll)*sin(self.pitch)*sin(self.yaw),-cos(self.roll)*cos(self.pitch))
        self.platform.axis=axis
        self.platform.up=up
        self.platform.length=1.0
        self.platform.width=0.65
        self.plat_arrow.axis=axis
        self.plat_arrow.up=up
        self.plat_arrow.length=0.8
        self.p_line.axis=axis
        self.p_line.up=up
        self.cil_roll.axis=(0.2*cos(self.roll),0.2*sin(self.roll),0)
        self.cil_roll2.axis=(-0.2*cos(self.roll),-0.2*sin(self.roll),0)
        self.cil_pitch.axis=(0.2*cos(self.pitch),0.2*sin(self.pitch),0)
        self.cil_pitch2.axis=(-0.2*cos(self.pitch),-0.2*sin(self.pitch),0)
        self.arrow_course.axis=(0.2*sin(self.yaw),0.2*cos(self.yaw),0)

        self.L1.text = str(roll) # in degrees
        self.L2.text = str(pitch)
        self.L3.text = str(yaw)