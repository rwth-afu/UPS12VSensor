#include "ServerProcess.h"
#include "Logger.h"
#include "IDataReader.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

ServerProcess::ServerProcess(shared_ptr<Logger> logger,
	unique_ptr<IDataReader> reader) :
	mLogger(move(logger)),
	mReader(move(reader)),
	mKill(false),
	mSD(-1)
{
	if (!mLogger)
	{
		throw invalid_argument("Invalid logger.");
	}

	if (!mReader)
	{
		throw invalid_argument("Invalid data reader.");
	}
}

ServerProcess::~ServerProcess()
{
	if (mSD > 0)
	{
		close(mSD);
	}
}

void ServerProcess::run(int port)
{
	if (mSD != -1)
	{
		mLogger->log(LogLevel::WARN, "Server already running.");
		return;
	}

	if (mKill)
	{
		return;
	}

	mLogger->log(LogLevel::DEBUG, "Starting server process.");

	mSD = socket(AF_INET, SOCK_STREAM, 0);
	if (mSD == -1)
	{
		throw runtime_error("Failed to create server socket.");
	}

	const int yes = 1;
	setsockopt(mSD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	try
	{
		setNonBlocking(mSD);
	}
	catch (const exception& ex)
	{
		close(mSD);
		throw ex;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(mSD, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
	{
		close(mSD);
		throw runtime_error("Failed to bind server socket.");
	}

	if (listen(mSD, 10) == -1)
	{
		close(mSD);
		throw runtime_error("Failed to listen on server socket.");
	}

	serverLoop();
}

void ServerProcess::stop()
{
	mKill = true;
}

void ServerProcess::serverLoop()
{
	mLogger->log(LogLevel::INFO, "Server started.");

	while (!mKill)
	{
		updateData();
		acceptClient();

		this_thread::sleep_for(chrono::milliseconds(1000));
	}

	mLogger->log(LogLevel::INFO, "Server stopped.");
}

void ServerProcess::updateData()
{
	SensorData data;
	mReader->read(data);

	ostringstream text;
	text << fixed << setfill('0') << setprecision(2) << data.UBat <<
		";" << data.IBat << ";" << data.UPsu << ";" << data.IPsu;

	mTextData = text.str();
}

void ServerProcess::acceptClient()
{
	sockaddr addr;
	socklen_t addrlen = sizeof(addr);

	int fd = accept(mSD, &addr, &addrlen);
	if (fd == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			mLogger->log(LogLevel::ERROR, "Failed to accept connection.");
		}

		return;
	}

	mLogger->log(LogLevel::INFO, "Client connected.");

	const auto len = write(fd, mTextData.c_str(), mTextData.size());
	if (len == -1)
	{
		mLogger->log(LogLevel::ERROR, "Failed to write data.");
	}

	close(fd);

	mLogger->log(LogLevel::INFO, "Client connection closed.");
}

void ServerProcess::setNonBlocking(int fd)
{
	auto flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		throw runtime_error("Failed to get flags.");
	}

	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
	{
		throw runtime_error("Failed to set flags.");
	}
}
