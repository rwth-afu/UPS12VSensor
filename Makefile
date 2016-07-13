CXX = g++
CXXFLAGS = -c -Wall -std=c++11 -pthread
LD = g++
LDFLAGS = -pthread -lconfig++

# Include local configuration to override defaults. Put all your changes
# into this file. This avoids cluttering the repository with your local
# settings.
-include Makefile.local

# Installation dir
ifndef PREFIX
	PREFIX = /usr/local
endif

# Configuration file
ifndef CONFIG_FILE
	CONFIG_FILE = /etc/I2CSwitchBoard.conf
endif

# Debug build
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -O2 -DNDEBUG
endif

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)
BIN = build/I2CSwitchBoard

.PHONY: all clean install systemd

all: $(BIN)

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -o $@ $<

build:
	mkdir -p $@

clean:
	rm -rf build/

install: all
	install -m 0755 $(BIN) $(PREFIX)/bin/
	install -m 0644 I2CSwitchBoard.conf $(CONFIG_FILE)
ifdef SYSTEMD
	install -m 0644 I2CSwitchBoard.default /etc/default/I2CSwitchBoard
	install -m 0644 I2CSwitchBoard.service /lib/systemd/system/
endif

