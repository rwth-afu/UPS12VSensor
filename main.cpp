#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <iostream>
#include <sstream>
#include <string>


#include "I2CSwitchBoard.h"

#define PROG_VERSION	"0.0.1"
#define COPYRIGHTZEILE1	"RasPagerDigi by DH3WR"
#define COPYRIGHTZEILE2	"DF6EF, Delissen 0.0.1"

#define TASTERDELAY_MS	50

int main(int argc, char** argv) {
    I2CSwitchBoard myI2CSwitchBoard(false);


	double readIBat();
	double readUBat();
	double readINetzteil();
	double readUNetzteil();
	
	while (1) {
		
		// Read ADC Values
		double ibat = myI2CSwitchBoard.readIBat();
		double ubat = myI2CSwitchBoard.readUBat();
		double inetzteil = myI2CSwitchBoard.readINetzteil();
		double unetzteil = myI2CSwitchBoard.readUNetzteil();
		cout << ubat << " V Bat  " << ibat << " A Bat  " << unetzteil << " V NT   " << inetzteil << " A NT  " << "\n";
		
		usleep(1000*200);
	}
    return 0;
}
