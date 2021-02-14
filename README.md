# Health Care Robot

IOT based healthcare robot for assisting patients of contagious disease. 
The project uses multiple microcontroller and a single board computer for its operation. 
The system uses firebase real time database.
Has advanced features such as delivering medicine to the patients autonomously, sending medical goods, checking patient vitals, upload vital data to the cloud. 
Has manual control, has a 5-DOF manual robotic arm which can be controlled using hand gestures. 
Has advanced obstacle avoidance capabilities.

## Video Demo: https://drive.google.com/file/d/1mQJPkHuqIsKNERTLCz55I2fx5MTER6VT/view?usp=sharing

# Technology Used
1. C++/Arduino for microcontroller
2. Raspberry PI as the main data retriever and processor
3. Python to retrieve and process data
4. HTML, CSS, JavaScript for the Internet of Things website
5. proportional integral derivative (PID), controller for line following and Left Hand Maze solving algorithm for path solving
6. Other sensors and actuators

# Features: 
1. reminding medicine for patient
2. traverse in the hospital following a line as guidance
3. 2D map of the environment for obstacle avoidance
4. Identify patient's hospital beds
5. check the heart-rate of patients and store it in database
6. Take temperature reading of patients
7. Web Based control panel
8. Mechanical robotic arm with bionic mimic control feature
9. Autonomous medicine delivery

## Main Robot Body
![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/robot.jpg)

## Hand Gesture Controller
![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/controller.jpg)

## Login Page of the IOT Website
![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/login.PNG)

## Bedlist of the available beds
This list is automatically uploaded from the robot, after the robot computes the arena

![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/bedlist.PNG)

## Medicine Entry panel
Medicine can be assigned for a particular bed with time and other parameters. The robot will automatically deliver those medicines at the correct time, at the correct bed.
![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/medEntry.PNG)

## Vital Sign Chart of Users
Users body temperature and Heart Rate readings are displayed as Line chart
![Alt Text](https://github.com/shaikh47/Health-Care-Robot/blob/master/screenshot/graph.PNG)
