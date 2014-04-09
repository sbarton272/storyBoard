# Control OLED via serial

import serial, time

OLED_PORT = 3

# Commands
MEDIA_INIT = 'FFB1'
ACK = 0x06

OLED = serial.Serial(OLED_PORT, 9600, timeout = 1)

try:
	OLED.write( 'FFCD0014001400143EA7' )
	# OLED.write(MEDIA_INIT)
	ack = OLED.read(10)
	print repr(ack)

except Exception as e:
	print e

finally:
	OLED.close()
	print 'done'
