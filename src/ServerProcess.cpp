#include "ServerProcess.h"
#include "DummyReader.h"
#include "I2CDataReader.h"
#include <array>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

static constexpr int LISTEN_BACKLOG = 5;

ServerProcess::ServerProcess(const Configuration& cfg) :
	mConfig(cfg),
	mShutdown(false)
{
	if (cfg.useDummyReader)
	{
		mReader = unique_ptr<IDataReader>(new DummyReader());
	}
	else
	{
		mReader = unique_ptr<IDataReader>(new I2CDataReader());
	}

	// Create server socket
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	mSD = socket(AF_INET, SOCK_STREAM, 0);
	if (mSD == -1)
	{
		throw runtime_error("Failed to create the server socket.");
	}

	int opt = 1;
	if (setsockopt(mSD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		close(mSD);
		throw runtime_error("Failed to set socket options.");
	}

	// Listen on any IPv4 address
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(mConfig.port);

	if (bind(mSD, reinterpret_cast<sockaddr*>(&addr), addrlen) == -1)
	{
		close(mSD);
		throw runtime_error("Failed to bind socket.");
	}

	if (listen(mSD, LISTEN_BACKLOG) == -1)
	{
		close(mSD);
		throw runtime_error("Failed to listen on server socket.");
	}
}

ServerProcess::~ServerProcess()
{
	if (mSD != -1)
	{
		close(mSD);
	}
}

void ServerProcess::run()
{
	if (mSD == -1)
	{
		throw runtime_error("Server socket not initialized.");
	}

	// Start updater thread
	thread updater(&ServerProcess::updateData, this);

	sockaddr addr;
	socklen_t addrlen = sizeof(addr);

	while (!mShutdown)
	{
		const auto sd = accept(mSD, &addr, &addrlen);
		if (sd == -1)
		{
			cerr << "Failed to accept connection." << endl;
			continue;
		}

		mLock.lock();
		const auto data = mTextData;
		mLock.unlock();

		// TODO Check if all data is written
		const auto written = write(sd, data.c_str(), data.size());
		if (written == -1)
		{
			cerr << "Failed to write data." << endl;
		}

		close(sd);
	}

	close(mSD);
	mSD = -1;
	updater.join();
}

void ServerProcess::updateData()
{
	const auto msec = chrono::milliseconds(mConfig.pollInterval);

	try
	{
		SensorData data;
		while (!mShutdown)
		{
			mReader->read(data);

			ostringstream text;
			text << fixed << setfill('0') << setprecision(2) <<
				data.UBat << ";" << data.IBat << ";" << data.UPsu <<
				";" << data.IPsu;

			mLock.lock();
			mTextData = text.str();
			mLock.unlock();

			this_thread::sleep_for(msec);
		}
	}
	catch (const exception& ex)
	{
		mShutdown = true;
	}
}
