import keras  #image processing
import numpy as np  #image processing
import cv2  #image processing
import imutils #
from keras.models import load_model
from keras.preprocessing.image import img_to_array
from keras.utils.generic_utils import CustomObjectScope

import serial #for serial communication between two device(monitor)
import time #time library

ser=serial.Serial("/dev/ttyACM0",9600) #open serial communication between raspberry and arduino #change ACM number as found from ls /dev/tty/ACM* 
ser.baudrate=9600 #communication speed between two platforms

while True:
    inp = raw_input("Your order, Doctor!")#function from raspberry pi #Input From Keyboard
    
    if (inp == "up"): 
        ser.write("up") #write at serical communication of arduino 
        time.sleep(2)#wait between two orders 
        print(ser.readline()) 
            
    if (inp == "down"):
        ser.write("down")
        time.sleep(2)
        print(ser.readline())
    
    if (inp == "pulse"):
        ser.write("pulse")
        print("Measuring pulse..")
        time.sleep(11)
        print(ser.readline())
        
    if (inp == "temperature"):
        ser.write("temp")
        print("Measuring temperature..")
        time.sleep(11)
        print(ser.readline())
            
    if (inp == "xray"):
        path = "/home/pi/Desktop/DNN X-Ray 2 classess/"#open directory classes 
        OUT_DIM = (96, 96)#size of displayed image 

        model = load_model(path + "final_model.h5")

        image = cv2.imread(path + "Test/Effusion.png")
        orig = image.copy()
        image = cv2.resize(image, (96, 96))
        image = image.astype("float") / 255.0
        image = img_to_array(image)
        image = np.expand_dims(image, axis=0)#expand array(image) on x-axis(axis=0) if y-axis(axis=1)

        abnormal, normal = model.predict(image)[0] #abnormal and normal are numeric 

        label = "Normal" if normal > abnormal else "abrnomal"
        print("{}".format(label))

        output = imutils.resize(orig, width=400)#resize original image to width 400
        cv2.putText(output, label, (10, 25),  cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2) #put text at the output 
        cv2.imshow("X-ray", output)#label (X-ray)that appear in the screen and the image(output)
        cv2.waitKey(0)

    
    time.sleep(2)


