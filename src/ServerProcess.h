#pragma once

#include <atomic>
#include <memory>
#include <string>

class IDataReader;
class Logger;

class ServerProcess
{
public:
	ServerProcess(std::shared_ptr<Logger> logger,
		std::unique_ptr<IDataReader> reader);

	ServerProcess(const ServerProcess& o) = delete;

	~ServerProcess();

	ServerProcess& operator=(const ServerProcess& o) = delete;

	void run(int port);

	void stop();

private:
	std::string getTextData();
	void acceptClient();

private:
	std::shared_ptr<Logger> mLogger;
	std::unique_ptr<IDataReader> mReader;
	std::atomic_bool mKill;
	int mSD;
};
