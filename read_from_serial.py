import serial
from solver import Solver
import time


if __name__ == '__main__':
    s = Solver()

    while True:
        with serial.Serial('/dev/ttyACM1', 9600, timeout=0.1, write_timeout=1) as ser:
            value = ''

            while ')' not in value:
                line = ser.readline().decode('utf-8').strip()

                if line != '':
                    print('line: {}'.format(line))
                    value = value + line

            print('value: {}'.format(value))
            s.string = value
            print('solve: {}'.format(s.solve()))
            
