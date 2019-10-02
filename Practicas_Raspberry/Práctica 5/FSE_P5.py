
# Lopez Santibanez Jimenez Luis Gerardo
# Ortiz Figuerosa Maria Fernanda
# Robles Uribe Karen Abril

import csv

fileName = input('Nombre archivo: ')
N = input('N: ')

with open('/media/pi/Abril/' +  str(fileName) + '.csv', 'w') as write_file:
    fieldnames = ['id','mesg','data']

    writer= csv.DictWriter(write_file, fieldnames=fieldnames)
    writer.writeheader()
    for i in range (int(N)+1):
        # with open('/media/pi/Abril/' + str(fileName) + '.csv', 'w') as write_file:
        writer.writerow({'id': str(i), 'mesg': 'Hola USB', 'data': 'data' + str(i)})
