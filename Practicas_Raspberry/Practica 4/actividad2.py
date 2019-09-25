#########################################
#	Practica 4			#
#	Actividad 2			#
#	Fundamentos de Sist. Embebidos	#
#########################################

import serial

serialData = serial.Serial("/dev/ttyS0", baudrate=115200, timeout = 1)	# Open port with baud rate

while True:

	data = serialData.read(14).strip()	# Read serialDataial port
	if data != b'':

		info = data.split(',')
		if len(info) == 4:

			e = info[0]
			i = info[1]
			b = info[2]
			v = info[3]

			if (e >= 0 or e <= 4095) and (v >= 0.0 or v <= 3.3):
				serialData.write('Numero entero: ' + str(e) + '\nIniciales: ' + str(i) + '\nBandera: ' + str(b) + '\nVoltaje' + str(v))

serialData.close()
