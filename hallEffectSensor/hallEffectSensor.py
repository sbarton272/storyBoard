# Script to read in hall effect sensor data and do real time plotting

import serial, time
import numpy as np
import matplotlib.pyplot as plt

MAX_TIME = 2500
ARDUINO_PORT = 4 # com5

FID = open('magnetData.txt', 'w')
ARDUINO = serial.Serial(ARDUINO_PORT, 9600, timeout = 1)

try:

    for t in xrange(0,MAX_TIME):
         
        senseVal = ARDUINO.readline().strip()
        if senseVal.isdigit():
            FID.write(senseVal + '\n')

except Exception as e:
    print e

finally:
    FID.close()
    ARDUINO.close()
    print 'done'