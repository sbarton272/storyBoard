# Collect light data

import serial, time

# ======================================
# Constants
# ======================================

# data files
FILENAME = 'data/redTest.txt'
DELIM = '\t'
DATA_HEADER = 'LED\tVAL\n'

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
COM_MULTI_SENSOR = 't'
COM_HANDSHAKE = 'h'
COM_MULTI_DONE = 'done'

# ======================================
# Helper Functs
# ======================================

def pollArdunio(command):
	"""pollArdunio(command) Write command to arduino and wait for response.
		Finally wait one sec before returning."""
	arduino.write(command)
	print '> ', arduino.readline(),
	time.sleep(ONE_SEC)

def pollSensor(color):
	"""pollSensor() Get multiple sensor readings and write to file"""
	pollArdunio(color)
	arduino.write(COM_MULTI_SENSOR)

	line = ''
	while line != COM_MULTI_DONE:
		line = arduino.readline().strip()
		if ( line == COM_MULTI_DONE ):
			break
		else:
			# write to file
			FID.write(color + DELIM + line + '\n')


# ======================================
# File I/O Set-up
# ======================================

FID = open(FILENAME, 'w')
FID.write(DATA_HEADER)

# ======================================
# Serial Set-up
# ======================================

arduino = serial.Serial(ARDUINO_PORT, timeout = 1)
print arduino.name, ' connected'
time.sleep(TWO_SEC)

# ======================================
# Gather data
# ======================================

try:
	# handshake
	pollArdunio(COM_HANDSHAKE)

	print 'testing functionality'
	pollArdunio(COM_BLUE)
	pollArdunio(COM_RED)
	pollArdunio(COM_GREEN)
	pollArdunio(COM_OFF)
	pollArdunio(COM_SENSOR)	

	print 'collecting data'	
	pollSensor(COM_BLUE)
	pollSensor(COM_RED)
	pollSensor(COM_GREEN)
	pollSensor(COM_OFF)

	print 'clean-up'
	pollArdunio(COM_OFF)
	arduino.close()

except Exception, e:
	raise e

finally:
	FID.close()
	print 'done'