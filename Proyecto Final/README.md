 PROYECTO FINAL GSM/GPRS
    Sensado de temperatura con alertas SMS

Alumnos:
     López Santibáñez Jiménez Luis Gerardo
     Ortiz Figueroa María Fernanda
     Robles Uribe Karen Abril

Asignatura:
     Fundamentos de Sistemas Embebidos

Descripción:
     Lectura de temperatura a través de un GPIO de la ESP32 proveniente del 
     sensor LM335 (10mV/K°), por lo que se lee el valor en mV,de tal forma 
     que se hace lo siguiente:
     *****
         Conversión para tener la temperatura en Kelvin y Centigrados para 
         enviar vía UART del ESP32 a la Computadora, para verificar la 
         información que se esta obteniendo del sensor
     *****
         Suponemos que se requiere enviar mensajes cada hora (1 minuto por 
         fines prácticos) con la temperatura sensada en ese momento
