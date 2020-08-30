import serial
import time
from random import randint
import csv
import turtle
import pyrebase
import json
from datetime import datetime

config={"apiKey": "AIzaSyDLEco45i2BszWHJb67eAc7qx8OZ-d9vC8",
        "authDomain": "iot-webapplication-47.firebaseapp.com",
        "databaseURL": "https://iot-webapplication-47.firebaseio.com",
        "projectId": "iot-webapplication-47",
        "storageBucket": "iot-webapplication-47.appspot.com",
        "messagingSenderId": "639567912933",
        "appId": "1:639567912933:web:e8140919d9641904e2590c"}

firebase = pyrebase.initialize_app(config)
db = firebase.database()

ser=serial.Serial('COM4',baudrate=9600,timeout=1)
time.sleep(3)

while 1:
   try:
      temp = ser.readline().decode('ascii')
      #ser.write(str(chr("data sent to arduino")))
      print(temp.strip())
      if(temp.strip() == "SEND DATA"):
         ser.write('a quick brown fox jumps over the lazy dog$'.encode('ascii'))
         temp = ser.readline().decode('ascii')
         time.sleep(0.1)
      elif(temp.strip() == "BED DATA"):
          ser.write('SEND DATA$'.encode('ascii')) #request bed data from arduino
          temp = ser.readline().decode('ascii')

          strTemp = temp.split('$')  # inserts the splitted words in a list
          for i in strTemp:
                data={'test':'data'}
                dataToSend='BED NUMBER_'+i
                path='/main/'+dataToSend+'/'
                result=db.child("main").child(dataToSend).set(data)
                print(dataToSend)
      print(temp.strip())
      temp=''
   except:
      print("EXCEPTION")


