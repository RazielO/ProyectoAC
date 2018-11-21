import serial
from solver import Solver
import time


if __name__ == '__main__':
    s = Solver()

    while True:
        with serial.Serial('/dev/ttyACM0', 9600, timeout=1, write_timeout=1) as ser:
            line = ser.readline().decode("utf-8").strip()
            if line != '':
                s.string = line
                print(s.string, ' = ', s.solve())

