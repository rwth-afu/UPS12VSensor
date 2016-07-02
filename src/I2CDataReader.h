#pragma once

#include "IDataReader.h"

class I2CDataReader : public IDataReader
{
public:
	I2CDataReader();

	I2CDataReader(const I2CDataReader& o) = delete;

	virtual ~I2CDataReader();

	I2CDataReader& operator=(const I2CDataReader& o) = delete;

	virtual void read(SensorData& data) override;

private:
	int readADC();
	void setMultiplexer(char channel);
	double readUBat();
	double readIBat();
	double readUPsu();
	double readIPsu();

private:
	int mADC = -1;
	int mMux = -1;
};
