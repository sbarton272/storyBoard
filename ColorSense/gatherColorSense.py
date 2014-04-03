# Collect light data

import serial, time
import numpy as np
import matplotlib.pyplot as plt

# set-up serial
ARDUINO_PORT = 4
# Commands
COM_RED = 'r'
COM_BLUE = 'b'
COM_GREEN = 'g'
COM_OFF = 'o'
COM_SENSOR = 's'


arduino = serial.Serial(ARDUINO_PORT)
print arduino.name, ' connected'

try:
	arduino.write('a')
	#arduino.write(COM_OFF)
	#arduino.write(COM_SENSOR)
	print 'written'
	while True
		print arduino.readln()

except Exception, e:
	raise e

finally:
	arduino.close()
	print 'done'