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

timeFlag=0

countBed=0
bedList=[]

listOfDict=[]
myDict={}


all_users = db.child("main").get()  #gets all the available beds
for user in all_users.each():
    #print(user.key()) #prints the bed names eg. BED NUMBER_1
    bedList.append(user.key())
    countBed=countBed+1 #counts the number of total beds

#print(bedList)
queue = [] #initialize queue


def multiplicacyChecker(x):  #counts the number of 'x' in the queue list
    global queue
    tempList=queue
    count=0
    for i in range(len(tempList)):
        if(str(tempList[i].split("$")[0]) == x):
            count=count+1
    return count



for k in bedList:
        try:
                all_users = db.child("main/" + k + "/med").get()
                for user in all_users.each():
                        myStr = str(user.val())
                        myStr = myStr.replace("\'", "\"")
                        myDict = json.loads(myStr)

                        lengthOfStr = len(k)
                        tempStrBed = k[11:lengthOfStr]
                        myDict["bed"] = tempStrBed

                        listOfDict.append(myDict)
                        #print("BED ",myDict["bed"])
                        #print("DOSAGE ",myDict["dosage"])
                        #print("MEAL ",myDict["meal"])
                        #print("MEDICINE ",myDict["medicine"])
                        #print("TIME ",myDict["time"])
                        #print("-------------------------")
                        myDict={}
        except:
                print("EMPTY")
print("RUNNING")

while 1:
        localtime = time.localtime(time.time())  # this portion calculates the current time
        minStr = str(localtime[4])
        hourStr = str(localtime[3])
        if (localtime[4] < 10):
                minStr = "0" + str(localtime[4])
        if (localtime[3] < 10):
                hourStr = "0" + str(localtime[3])
        realTimeStr = str(hourStr + ":" + minStr)

        realTimeStr="18:54"
        if(localtime[5]==1 and timeFlag==0): #program checks every second if there is any available medicine to give
                for i in range(len(listOfDict)):  # iterate through the list to find if its time to deliver medicine
                        if (listOfDict[i]["time"] == realTimeStr):
                                tempList = listOfDict[i]["bed"]+'$'+listOfDict[i]["medicine"] + "/" + listOfDict[i]["dosage"] + "/" + listOfDict[i]["meal"]

                                queue.append(tempList)
                                timeFlag = 1  # set flag to 1 to prevent code block from coming here again before 1 second

        #remove multiple medicine for the same bed at the same time
        #merge those medicine entries into one entry
        #after polishing, use this code only when a new medicine entry is added
        #, as we dont need to check duplicates all the time
        if(localtime[5]==1):
                for y in range(len(listOfDict)):
                        varTemp = multiplicacyChecker(str(listOfDict[y]["bed"]))  # call to check the multiplicacy in queue list

                        tempQueue=[]
                        lis = []
                        for k in range(len(queue)):
                                lis.append(queue[k].split("$")[0])

                        if (varTemp > 1):  # if the number of entries for that bed is greater than one
                                varIndex = lis.index(str(listOfDict[y]["bed"]))
                                tempList = ""
                                for x in range(varTemp):
                                        tempList = tempList+queue.pop(varIndex).split("$")[1]+"-"
                                queue.append(str(listOfDict[y]["bed"])+'$'+tempList)
                                tempList=""

        if(len(queue)!=0): #check if queue is not empty, if queue has item then send it to arduino if possible
                        print(queue.pop(0)) #popping item

        if(localtime[5]==2): #reset the flag at 2 second mark
                timeFlag=0 #reset the flag

        realTimeStr = "18:53"
