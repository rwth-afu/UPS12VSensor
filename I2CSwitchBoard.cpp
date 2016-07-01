#include "I2CSwitchBoard.h"

I2CSwitchBoard::I2CSwitchBoard(bool skipSetup) {
    if (skipSetup) {
        cout << "WARNING: Skipping I2C Communication Setup!" << endl;
        return;
    }

    cout << "Setting up I2C Communication with extension boards..." << endl;
    system("gpio load i2c");
    wiringPiSetup() ;

	
	
    // ADC
    fdadc = wiringPiI2CSetup(I2CADDRESS_ADC);
    // Init ADC into auto run mode, 4 ksps
    wiringPiI2CWriteReg8(fdadc, 0x02, 0xE0);


    // 4 Bit I2C Extension
    fdExt4bit = wiringPiI2CSetup(I2CADDRESS_EXT4BIT);
    // Init 4 Bit Mux: All Ports to Output Mode
    wiringPiI2CWriteReg8(fdExt4bit, PCA9536CONFREG, 0x00);
}


int I2CSwitchBoard::readADCValue() {
    int myadcvalue = wiringPiI2CReadReg16(fdadc, 0x00);
    // swap lower and higher bit
    int myadcLowerByte = (myadcvalue & 0xFF00) >> 8;
    int myadcHigherByte = (myadcvalue & 0x00FF) << 8;

    return (myadcHigherByte | myadcLowerByte);
}

void I2CSwitchBoard::setMeasurementMultiplexer(char channel) {
	wiringPiI2CWriteReg8(fdExt4bit, PCA9536OUTPUTREG, (channel));
	// Wait some time to let the voltages get stable
	delay(50);
}

int I2CSwitchBoard::readIBatRaw() {
    setMeasurementMultiplexer(PCA9536IBAT);
    return readADCValue();
}

int I2CSwitchBoard::readUBatRaw() {
	setMeasurementMultiplexer(PCA9536UBAT);
	return readADCValue();
}

int I2CSwitchBoard::readINetzteilRaw() {
	setMeasurementMultiplexer(PCA9536INETZTEIL);
	return readADCValue();
}

int I2CSwitchBoard::readUNetzteilRaw() {
	setMeasurementMultiplexer(PCA9536UNETZTEIL);	
	return readADCValue();
}


double I2CSwitchBoard::readIBat() {
    double res;
    res = (((double)readIBatRaw() / 4096 * 5) - 0.495) * 0.4;
	if (res < 0) { res = 0;}
	return res;
}

double I2CSwitchBoard::readUBat() {
    double res;
    res = ((double)readUBatRaw() / 4096 * 5) / 10000 * (10000 + 33000);
    return res;
}

double I2CSwitchBoard::readINetzteil() {
    double res;
    res = (((double)readINetzteilRaw() / 4096 * 5) - 0.495) * 0.4;
	if (res < 0) { res = 0;}
    return res;
	
}

double I2CSwitchBoard::readUNetzteil(){
    double res;
    res = ((double)readUNetzteilRaw() / 4096 * 5) / 10000 * (10000 + 33000);
    return res;
	
}


string I2CSwitchBoard::doubleValueToString(double val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

string I2CSwitchBoard::shortUnsignedIntToString(short unsigned int val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

string I2CSwitchBoard::boolToString(bool val) {
    return val ? "Yes" : "No";
}