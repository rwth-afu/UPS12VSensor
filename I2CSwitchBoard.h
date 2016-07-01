#pragma once

#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#define I2CADDRESS_ADC		0x51
#define I2CADDRESS_EXT4BIT	0x41

#define PCA9536OUTPUTREG	0x01
#define PCA9536CONFREG		0x03

#define PCA9536IBAT			0x00
#define PCA9536UNETZTEIL	0x01
#define PCA9536UBAT			0x02
#define PCA9536INETZTEIL	0x03


using namespace std;

class RaspagerDigiExtension {
private:
    int fdadc;
    int fdExt4bit;
    int readADCValue();
    
	int readIBatRaw();
	int readUBatRaw();
	int readINetzteilRaw();
    int readUNetzteilRaw();
	void setMeasurementMultiplexer(char channel);
	
public:
    RaspagerDigiExtension(bool skipSetup);
    double readIBat();
	double readUBat();
	double readINetzteil();
	double readUNetzteil();


    static string doubleValueToString(double val);
    static string shortUnsignedIntToString(short unsigned int val);
    static string boolToString(bool val);
};
