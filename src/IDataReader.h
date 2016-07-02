#pragma once

struct SensorData
{
	double UBat;
	double IBat;
	double UPsu;
	double IPsu;
};

class IDataReader
{
public:
	virtual ~IDataReader() = default;

	virtual void read(SensorData& data) = 0;
};
