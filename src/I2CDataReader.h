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
	int readADC() const;
	void setMultiplexer(char channel) const;
	double readUBat() const;
	double readIBat() const;
	double readUPsu() const;
	double readIPsu() const;

private:
	int mADC = -1;
	int mMux = -1;
};
