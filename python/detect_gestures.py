import time

class DetectGesture:

    def __init__(self):
        self.detected = False
        self.measuresRoll = []
        self.measuresPitch = []
        self.measuresYaw = []
        self.detectionTime = 0
        
    def addMeasures(self, roll, pitch, yaw):
            
        if not self.detected:
            self.measuresRoll.append(roll)
            self.measuresPitch.append(pitch)
            self.measuresYaw.append(yaw)
            
            length = len(self.measuresRoll)
            
            if (length >= 80):
                for i in range(1):
                    if (self.measuresRoll[i] > -15 and self.measuresRoll[i] < 15) and ((self.measuresRoll[length-i-1] > 165 and self.measuresRoll[length-i-1] < 180) or (self.measuresRoll[length-i-1] < -165 and self.measuresRoll[length-i] > -180)):
                        if i == 0:
                            self.detected = True
                            self.measuresRoll = []
                            self.detectionTime = time.time()
                    else:
                        self.measuresRoll.pop(0)
                        break

    def isDetected(self):
        if (time.time() - self.detectionTime) > 10:
            self.detected = False
        return self.detected
