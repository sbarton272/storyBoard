# Collect light data

import serial, time
import numpy as np
import matplotlib.pyplot as plt

# set-up serial
ARDUINO_PORT = 4
ONE_SEC = 1
TWO_SEC = 2

# Commands
COM_RED = 'r'
COM_BLUE = 'b'
COM_GREEN = 'g'
COM_OFF = 'o'
COM_SENSOR = 's'
COM_HANDSHAKE = 'h'


def pollArdunio(command):
	arduino.write(command)
	print '> ', arduino.readline(),
	time.sleep(ONE_SEC)

arduino = serial.Serial(ARDUINO_PORT, timeout = 1)
print arduino.name, ' connected'
time.sleep(TWO_SEC)

# handshake
arduino.write(COM_HANDSHAKE)
print arduino.readline(),

try:
	pollArdunio(COM_BLUE)
	pollArdunio(COM_RED)
	pollArdunio(COM_GREEN)
	pollArdunio(COM_OFF)
	pollArdunio(COM_SENSOR)		

except Exception, e:
	raise e

finally:
	arduino.close()
	print 'done'