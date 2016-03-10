CC=g++ -g
CXXFLAGS=-c -Wall -I include -std=c++0x
LDFLAGS= -lboost_system -lboost_filesystem -lgphoto2 -lmicrohttpd -lexif -lpthread
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
EXECUTABLE=CameraControllerApi
RESOURCES= resources/error_messages.xml resources/settings.xml webif

all: dirs $(CPP_FILES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

dirs:
	@mkdir -p bin obj
	@cp -R $(RESOURCES) bin


.PHONY: clean
clean:
	$(RM) -rf $(EXECUTABLE) obj bin
