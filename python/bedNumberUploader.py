import serial
import time
from random import randint
import csv
import turtle
from firebase import firebase
import json
from datetime import datetime

firebase = firebase.FirebaseApplication("https://iot-webapplication-47.firebaseio.com/");
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
                data={'name':'blank'}
                dataToSend='BED NUMBER_'+i
                path='/main/'+dataToSend+'/'
                result=firebase.put(path, "test","blank")
                print(dataToSend)
      print(temp.strip())
      temp=''
   except:
      print("EXCEPTION")


