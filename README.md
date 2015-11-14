CameraControllerApi
===================
The CameraControlerApi is an attempt to control a DSLR via REST functionality. At the moment is it possible 
to change the camera settings (ISO, aperture, time), take pictures and stream the live view of the camera (only tested with a Nikon D90).

###Web-Interface###

`http://device_ip:port/webif/`


###Demonstration###

[![a demonstration for the CameraControlerApi](http://img.youtube.com/vi/tkMP7_gnoiU/3.jpg)](http://www.youtube.com/watch?v=tkMP7_gnoiU)


How to use
-----------
You will get all valid data for a command from the "list" action.

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

<small>Returns a file with connection data. The command will open a socket which allows you to connect to get the stream data.</small>



**end liveview**

`http://device_ip:port/capture?action=live&value=end`



###File system###

**list of the available images on camera**

`http://device_ip:port/fs?action=list`



**get an image**

`http://device_ip:port/fs?action=get&value=filename.jpg&path=/path/to/file`




Each method will response with a JSON file. If you want a XML response you have to put the command "&amp;type=xml" on the end of the upper commands



##Dependencies##
```apt-get install libboost-dev libboost-system-dev libmicrohttpd-dev libghoto2-dev libexif-dev```

+ libgphoto
+ libboost 
+ libboost-system
+ libmicrohttpd
+ libexif
