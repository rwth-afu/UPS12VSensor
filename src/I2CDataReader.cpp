#include "I2CDataReader.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace Devices
{
static constexpr char ADC = 0x51;
static constexpr char MUX = 0x41;
}

namespace PCA9536
{
static constexpr char REG_OUT = 0x01;
static constexpr char REG_CONF = 0x03;
static constexpr char SENS_IBAT = 0x00;
static constexpr char SENS_UPSU = 0x01;
static constexpr char SENS_UBAT = 0x02;
static constexpr char SENS_IPSU = 0x03;
}

I2CDataReader::I2CDataReader()
{
	mADC = open("/dev/i2c-1", O_RDWR);
	if (mADC == -1)
	{
		throw runtime_error("Failed to open /dev/i2c-1.");
	}

	if (ioctl(mADC, I2C_SLAVE, Devices::ADC) < 0)
	{
		close(mADC);
		throw runtime_error("Failed to open ADC connection.");
	}

	// Init ADC into auto run mode, 4 ksps
	if (i2c_smbus_write_byte_data(mADC, 0x02, 0xE0) < 0)
	{
		close(mADC);
		throw runtime_error("Failed to init ADC.");
	}

	mMux = open("/dev/i2c-1", O_RDWR);
	if (mMux == -1)
	{
		close(mADC);
		throw runtime_error("Failed to open /dev/i2c-1 for extension.");
	}

	if (ioctl(mMux, I2C_SLAVE, Devices::MUX) < 0)
	{
		close(mADC);
		close(mMux);
		throw runtime_error("Failed to open EXT connection.");
	}

    // Set all ports to output mode
    if (i2c_smbus_write_byte_data(mMux, PCA9536::REG_CONF, 0x00) < 0)
	{
		close(mADC);
		close(mMux);
		throw runtime_error("Failed to init EXT.");
	}
}

I2CDataReader::~I2CDataReader()
{
	if (mADC != -1)
	{
		close(mADC);
	}

	if (mMux != -1)
	{
		close(mMux);
	}
}

void I2CDataReader::read(SensorData& data)
{
	data.UBat = readUBat();
	data.IBat = readIBat();
	data.UPsu = readUPsu();
	data.IPsu = readIPsu();
}

int I2CDataReader::readADC() const
{
	const auto value = i2c_smbus_read_word_data(mADC, 0x00);
	if (value < 0)
	{
		throw runtime_error("Failed to read ADC value.");
	}

	const auto adcLowerByte = (value & 0xFF00) >> 8;
    const auto adcHigherByte = (value & 0x00FF) << 8;

    return (adcHigherByte | adcLowerByte);
}

void I2CDataReader::setMultiplexer(char channel) const
{
	if (i2c_smbus_write_byte_data(mMux, PCA9536::REG_OUT, channel) < 0)
	{
		throw runtime_error("Failed to change mux channel.");
	}

	// Wait some time to let the ADC values get stable
	this_thread::sleep_for(chrono::milliseconds(50));
}

double I2CDataReader::readUBat() const
{
	setMultiplexer(PCA9536::SENS_UBAT);
	auto value = static_cast<double>(readADC());

	return (value / 4096.0 * 5.0) / 10000.0 * (10000.0 + 33000.0);
}

double I2CDataReader::readIBat() const
{
	setMultiplexer(PCA9536::SENS_IBAT);
	auto value = static_cast<double>(readADC());

	value = ((value / 4096.0 * 5.0) - 0.495) / 0.4;
	if (value < 0.0)
	{
		value = 0.0;
	}

	return value;
}

double I2CDataReader::readUPsu() const
{
	setMultiplexer(PCA9536::SENS_UPSU);
	auto value = static_cast<double>(readADC());

	return (value / 4096.0 * 5.0) / 10000.0 * (10000.0 + 33000.0);
}

double I2CDataReader::readIPsu() const
{
	setMultiplexer(PCA9536::SENS_IPSU);
	auto value = static_cast<double>(readADC());

	value = ((value / 4096.0 * 5.0) - 0.495) / 0.4;
	if (value < 0.0)
	{
		value = 0.0;
	}

	return value;
}
