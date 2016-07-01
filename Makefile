CXX = g++
CXXFLAGS = -c -Wall
LD = g++
LDFLAGS = -lwiringPi

BIN = build/bat
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=build/%.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -o $@ $<

build:
	mkdir $@

clean:
	rm -rf build/
