import serial
import time

ser=serial.Serial("/dev/ttyACM0",9600)  #change ACM number as found from ls /dev/tty/ACM*
ser.baudrate=9600



while True:
    inp = raw_input("Order?")
    
    if (inp == "up"):
        ser.write(inp)
        time.sleep(2)
        print(ser.readline())
        
        
    if (inp == "down"):
        ser.write(inp)
        time.sleep(2)
        print(ser.readline())
    
    
    if (inp == "pulse"):
        ser.write(inp)
        print("Measuring pulse..")
        time.sleep(11)
        print(ser.readline())
        
        
    if (inp == "temp"):
        ser.write(inp)
        print("Measuring temperature..")
        time.sleep(11)
        print(ser.readline())
        
    
    time.sleep(2)
