# Control OLED via serial

import serial, time, sys

OLED_PORT = 3

BYTE_SIZE = 1
WORD_SIZE = 2*BYTE_SIZE # 2 bytes
CMD_DELAY = .5

# Commands
MEDIA_INIT 		= [0xFF,0xB1]
SS_TIMEOUT 		= [0x00, 0x0C, 0x00, 0x00] # stop scrolling
CLEAR_SCREEN 	= [0xFF, 0xD7]
SET_SECT_0 		= [0xFF, 0xB8, 0x00, 0x00, 0x00, 0x00]
RUN_VIDEO 		= [0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00]  
VIDEO_FRAME_0 	= [0xFF, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
VIDEO_FRAME_1 	= [0xFF, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01]
ACK = 0x06

OLED = serial.Serial(OLED_PORT, 9600, timeout = 1)

def writeAck(cmd):
	OLED.write(cmd)
	ack = OLED.read(BYTE_SIZE)
	if ack != chr(ACK):
		cmdStr = ' '.join([repr(b) for b in cmd])
		print 'Problem with', cmdStr, 'got:', repr(ack)
	else:
		time.sleep(CMD_DELAY)

try:

	writeAck(SS_TIMEOUT)
	writeAck(CLEAR_SCREEN)

	OLED.write(MEDIA_INIT)
	ack = OLED.read(BYTE_SIZE)
	value = OLED.read(WORD_SIZE)
	print 'init', repr(ack), repr(value)
	time.sleep(CMD_DELAY)
	
	writeAck(RUN_VIDEO)
	writeAck(RUN_VIDEO)
	writeAck(RUN_VIDEO)
	writeAck(RUN_VIDEO)
	writeAck(VIDEO_FRAME_0)
	writeAck(VIDEO_FRAME_1)
	writeAck(VIDEO_FRAME_0)
	time.sleep(5)

except Exception as e:
	print e

finally:
	OLED.close()
	print 'done'
