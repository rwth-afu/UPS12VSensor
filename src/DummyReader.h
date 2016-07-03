#pragma once

#include "IDataReader.h"
#include <random>

class DummyReader : public IDataReader
{
public:
	DummyReader() :
		mVolts(11.8, 12.8),
		mCurrents(0.0, 4.0)
	{
		std::random_device rd;
		mRNG = std::mt19937(rd());
	}

	virtual ~DummyReader() = default;

	virtual void read(SensorData& data) override
	{
		data.UBat = mVolts(mRNG);
		data.IBat = mCurrents(mRNG);
		data.UPsu = mVolts(mRNG);
		data.IPsu = mCurrents(mRNG);
	}

private:
	std::mt19937 mRNG;
	std::uniform_real_distribution<double> mVolts;
	std::uniform_real_distribution<double> mCurrents;
};
