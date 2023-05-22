import serial
arduino = serial.Serial(port='COM16', baudrate=250000)

from pathlib import Path
mypath = Path(__file__).with_name("data.txt")

with open(mypath) as my_file:
   for line in my_file:
        signals = line.rstrip()
        sync = arduino.readline()
        print("Got data:",sync)
        if sync == b"xyz123\r\n":
            arduino.write(line)
            print("Sending data:",line)

