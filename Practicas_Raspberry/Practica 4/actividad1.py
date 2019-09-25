#########################################
#	      Practica 4		#
#	     Actividad 1		#
#    Fundamentos de Sist. Embebidos	#
#########################################

import serial

serialData = serial.Serial("/dev/ttyS0", baudrate=115200, timeout=1)

for i in range(16):
	serialData.write((str(i) + ':\tFSE 2020-1 Comunicacion UART RPi - FSE\n').encode())

serialData.close()
