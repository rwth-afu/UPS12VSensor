#include "ServerProcess.h"
#include "Logger.h"
#include "IDataReader.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
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

	mLogger->log(LogLevel::INFO, "Server started.");

	while (!mKill)
	{
		acceptClient();
	}

	mLogger->log(LogLevel::INFO, "Server stopped.");
}

void ServerProcess::stop()
{
	mKill = true;
}

string ServerProcess::getTextData()
{
	SensorData data;
	mReader->read(data);

	ostringstream text;
	text << fixed << setfill('0') << setprecision(2) << data.UBat <<
		";" << data.IBat << ";" << data.UPsu << ";" << data.IPsu;

	return text.str();
}

void ServerProcess::acceptClient()
{
	sockaddr addr;
	socklen_t addrlen = sizeof(addr);

	int fd = accept(mSD, &addr, &addrlen);
	if (fd == -1)
	{
		if (errno != EINTR)
		{
			mLogger->log(LogLevel::ERROR, "Failed to accept connection.");
		}

		return;
	}

	mLogger->log(LogLevel::DEBUG, "Client connected.");

	try
	{
		const auto txt = getTextData();
		const auto len = write(fd, txt.c_str(), txt.size());
		if (len == -1)
		{
			mLogger->log(LogLevel::ERROR, "Failed to write data.");
		}
	}
	catch (const exception& ex)
	{
		mLogger->log(LogLevel::ERROR, ex.what());
	}

	close(fd);

	mLogger->log(LogLevel::DEBUG, "Client connection closed.");
}
