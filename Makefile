CC=g++ -g
CXXFLAGS=-c -Wall -I include -std=c++0x
LDFLAGS= -lboost_system  -lgphoto2 -lmicrohttpd -lpthread -lexiv2
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
EXECUTABLE=CameraControllerApi
RESOURCES= resources/error_messages.xml resources/settings.xml webif

all: dirs $(CPP_FILES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

dirs:
	@mkdir -p bin obj
	@cp -R $(RESOURCES) bin


.PHONY: clean
clean:
	$(RM) -rf $(EXECUTABLE) obj bin
