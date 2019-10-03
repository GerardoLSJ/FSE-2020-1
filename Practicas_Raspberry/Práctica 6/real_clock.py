#!/usr/bin/env python

import sys
import time
import datetime

import SDL_DS1307
import csv

fileName = raw_input('Nombre archivo: ')
print('ok..')
with open('/media/pi/Abril/' +  str(fileName) + '.csv', 'w') as write_file:
	fieldnames = ['msg','clk']

	writer= csv.DictWriter(write_file, fieldnames=fieldnames)
	writer.writeheader()

	# Main Program
	print ("\nProgram Started at:"+ time.strftime('%Y-%m-%d %H:%M:%S'))
	writer.writerow({'msg': 'Program Started at ', 'clk': str(time.strftime('%Y-%m-%d %H:%M:%S'))})


	filename = time.strftime("%Y-%m-%d%H:%M:%SRTCTest") + ".txt"
	starttime = datetime.datetime.utcnow()

	ds1307 = SDL_DS1307.SDL_DS1307(1, 0x68)
	ds1307.write_now()


	# Main Loop - sleeps 10 minutes, then reads and prints values of all clocks
	while True:

		currenttime = datetime.datetime.utcnow()
		deltatime = currenttime - starttime

		print ('\n>> CLOCK: Raspberry Pi 3b+ \t' + time.strftime('%Y-%m-%d %H:%M:%S'))
		writer.writerow({'msg': '\n>> CLOCK: Raspberry Pi 3b+ \t ', 'clk': str(time.strftime('%Y-%m-%d %H:%M:%S'))})

		print ('>> CLOCK: DS1307 \t\t%s' % ds1307.read_datetime())
		writer.writerow({'msg': '>> CLOCK: DS1307 \t\t', 'clk': str(ds1307.read_datetime())})

		time.sleep(1.0)
