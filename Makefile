CXX = g++
CXXFLAGS = -c -Wall -std=c++11 -pthread
LD = g++
LDFLAGS = -pthread

# Include local configuration to override defaults. Put all your changes
# into this file. This avoids cluttering the repository with your local
# settings.
-include Makefile.local

# Configuration file location
ifndef CONFIG_FILE_PATH
	CONFIG_FILE_PATH = "/etc/default/I2CSwitchBoard.conf"
endif

CXXFLAGS += -DCONFIG_FILE_PATH=$(CONFIG_FILE_PATH)

# Debug build
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -O2 -DNDEBUG
endif

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)
BIN = build/I2CSwitchBoard

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -o $@ $<

build:
	mkdir -p $@

clean:
	rm -rf build/
