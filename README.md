CameraControllerApi
===================
The CameraControlerApi is an attempt to control a DSLR via REST functionality. At the moment is it possible 
to change the camera settings (ISO, aperture, time), shoot pictures and stream the live view of the camera.

How to use
-----------
You will get all valid data for a command from the "List" action. 

###Settings###

**List the configuration with validate values**

`http://device_ip:port/settings?action=list`



**ISO**

`http://device_ip:port/settings?action=iso&amp;value=200`



**Aperture**

`http://device_ip:port/settings?action=aperture&amp;value=f/22`



**Shutter Speed**

`http://device_ip:port/settings?action=speed&amp;value=1/1000`



**Whitebalance**

`http://device_ip:port/settings?action=whitebalance&amp;value=Cloudy`



###Capture###

**take a picture**

`http://device_ip:port/capture?action=shot`



**autofocus**

`http://device_ip:port/capture?action=autofocus`



**start liveview**

`http://device_ip:port/capture?action=live&value=start`

<small>Returns a file with connection data. The command will open a socket with which you can connect to get the stream data.</small>



**end liveview**

`http://device_ip:port/capture?action=live&value=end`



Each method will response with a file in json format. If you want an XML response you have to put the command "&amp;type=xml" on the end of the upper commands


##Dependencies##
+ libgphoto2-2.5.2
+ libboost 
+ libboost-system
+ libmicrohttpd
