import serial
from solver import Solver
import time


if __name__ == '__main__':
    s = Solver()  # Objeto para resolver la operación dada

    while True:  # Ciclo infinito
        with serial.Serial('/dev/ttyACM1', 9600, timeout=0.1, write_timeout=1) as ser:  # Abrir el puerto serial
            value = ''  # Cadena que se va a leer

            while ')' not in value:  # Leer hasta que encuentre un ')'
                line = ser.readline().decode('utf-8').strip()  # Decodificar los bytes del puerto serial en un string

                if line != '':  # Si no está vacía
                    value = value + line  # Concatenar la lectura a la cadena

            s.string = value  # Ingresar la operación al objeto para que resuelva la operación
            ser.write("{}\n".format(s.solve()).encode('utf-8'))  # Resolver la operación y escribir el resultado en el puerto serial
            
