CC=g++ -g
CFLAGS=-c -Wall
LDFLAGS= -lboost_system -lgphoto2 -lmicrohttpd
SOURCES=main.cpp Api.cpp Base64.cpp CameraController.cpp Command.cpp Server.cpp Settings.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=CameraControllerApi

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean: 
	$(RM) $(EXECUTABLE) $(OBJECTS)