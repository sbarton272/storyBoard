# Control OLED via serial

import serial, time, sys
from getch import getch

OLED_PORT = 3

BYTE_SIZE = 1
WORD_SIZE = 2*BYTE_SIZE # 2 bytes

# Commands
MEDIA_INIT = [0xFF,0xB1]
SS_TIMEOUT = [0x00, 0x0C, 0x00, 0x00] # stop scrolling
CLEAR_SCREEN = [0xFF, 0xD7]
RUN_VIDEO = [0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00]  
VIDEO_FRAME_0 = [0xFF, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00]
VIDEO_FRAME_1 = [0xFF, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x01]
ACK = 0x06

OLED = serial.Serial(OLED_PORT, 9600, timeout = 1)

print getch()
print 'done'

def writeAck(cmd):
	OLED.write(SS_TIMEOUT)
	ack = OLED.read(BYTE_SIZE)
	if ack != chr(ACK):
		cmdStr = ' '.join([repr(b) for b in cmd])
		print 'Problem with', cmdStr, 'got:', repr(ack)

try:
	writeAck(SS_TIMEOUT)
	
	writeAck(CLEAR_SCREEN)

	OLED.write(MEDIA_INIT)
	ack = OLED.read(BYTE_SIZE)
	value = OLED.read(WORD_SIZE)
	print repr(ack), repr(value)
	
	# key control
	for x in xrange(0,10): # cmd != ENTER:

		print 

		# if number run video that number of times
		# if cmd is number:
		# 	writeAck(RUN_VIDEO)

		# 	writeAck(CLEAR_SCREEN)

		# 	writeAck(VIDEO_FRAME_0)
		# 	writeAck(VIDEO_FRAME_1)

except Exception as e:
	print e

finally:
	OLED.close()
	print 'done'
