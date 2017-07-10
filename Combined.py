import sys
import serial
import datetime
from time import sleep
import time
import picamera
import subprocess

filecounter = 1
counter = 12

ser = serial.Serial('/dev/ttyACM0', 9600)
while True:
        # Read a line and convert it from b'xxx\r\n' to xxx
        print('Waiting for data.....')
        data = ser.readline().decode('utf-8')[:-2]
        # Create Date / Time stamp
        now_string = datetime.datetime.now().strftime("%d/%m/%y %H:%M:%S")
        # Print to console
        print(now_string)
        print(data)
        # Split the data into variables to get ready to submit to Mohawk
        Int_Temp, Int_humid, Temp, Anem, Pyran, Hum, Bar = data.split(',')
        # Send to PHP file with arguments (Is a test file that shows in terminal for now)
        print('Result from Mohawk:')
        subprocess.call(["php","-f","/var/www/html/db.php", Int_Temp, Int_humid, Temp, Anem, Pyran, Hum, Bar])
        # Loop to take picture once an hour otherwise capture a picture to the capture directory
        if counter < 12:
                counter = counter + 1
                # Save capture to web directory for local webpage
                with picamera.PiCamera() as camera:
                        camera.resolution = (1024, 768)
                        camera.start_preview()
                        #Add Current date and time to picture
                        camera.annotate_text = now_string + ' ' + data
                        time.sleep(5)
                        camera.capture('/var/www/html/capture.jpg')
                        print('Capturing to web directory')
                        print ('Picture Saved')
                        
        else:
                with picamera.PiCamera() as camera:
                        camera.start_preview()
                        #Add Current date and time to picture
                        camera.annotate_text = now_string + ' ' + data
                        time.sleep(2)
                        #  Save for submit?
                        fileName = '/home/pi/Desktop/Pics' + time.strftime("%H") + '_hours_img.jpg'
                        for filename in camera.capture_continuous(fileName):
                                print('Capturing %s' % filename)
                                print ('Picture Saved')
                                break
                        # Save capture to web directory for local webpage
                        camera.capture_continuous("/var/www/html/capture.jpg")
                        print('Captured to web directory')
                        print ('Picture Saved')

                counter = 0
                if filecounter < 24:
                        filecounter = filecounter + 1
                else:
                        filecounter = 1
        print(counter)
