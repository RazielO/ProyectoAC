import serial

if __name__ == "__main__":
    arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600)
    print(arduino.is_open)
    arduino.write(b'hello')
    arduino.close()