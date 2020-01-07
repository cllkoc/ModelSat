import picamera
import time
import numpy
import serial
import os 
from PIL import Image, ImageDraw, ImageFont
Image.Image.tostring = Image.Image.tobytes


f = open("/home/pi/Desktop/Telemetri.txt",'w')
f.close()
ser = serial.Serial ("/dev/ttyS0", baudrate=9600,timeout=0.2)#Open port with baud rate
text=""
data2=""
while 1:
    text = ser.read()
    if text=='x':
           print "Aktarim basladi"
           break

# Video Resolution
VIDEO_HEIGHT = 480
VIDEO_WIDTH = 640



with picamera.PiCamera() as camera:
   camera.resolution = (VIDEO_WIDTH, VIDEO_HEIGHT)
   camera.framerate = 30
   camera.led = False
   camera.start_preview()
   camera.start_recording('T_MUY_2019_44106.h264')
   time.sleep(1)
   try:
      while True:
                  f = open("/home/pi/Desktop/Telemetri.txt",'a')
                  if ser.inWaiting() > 0:
                                   data2 = ser.read()
                                   if data2=='j':
                                          camera.stop_recording()  
                                          os.system("sudo shutdown -h now")  
                                   data = ser.readline()
                                   print data
                                   f.write(data)
                                   f.close()
                  camera.wait_recording(0.9)

   finally:
      camera.remove_overlay(overlay)
      camera.stop_recording()