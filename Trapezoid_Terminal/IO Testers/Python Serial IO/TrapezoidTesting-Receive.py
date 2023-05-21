import serial

arduino = serial.Serial(port='COM16', baudrate=250000)

with open("data.txt" , mode="wb") as my_file:
    while True:
        data = arduino.readline()
        print("Got back:",data)
        my_file.write(data)


my_file.close()