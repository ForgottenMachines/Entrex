import serial

arduino = serial.Serial(port='COM16', baudrate=250000)

with open("data.txt" , mode="wb") as my_file:
    for line in my_file:
        while True:
            sync = arduino.readline()
            print("Waiting for sync")
            if sync == "xyz123":
                print(line.rstrip())
            
            
            break
my_file.close()