#pragma once

#include <atomic>
#include <memory>
#include <mutex>

class IDataReader;

class ServerProcess
{
public:
	struct Configuration
	{
		int port;
		int pollInterval;
		bool useDummyReader;
	};

public:
	ServerProcess(const Configuration& cfg);

	ServerProcess(const ServerProcess& o) = delete;

	~ServerProcess();

	ServerProcess& operator=(const ServerProcess& o) = delete;

	void run();

	void stop();

private:
	void updateData();

private:
	const Configuration mConfig;
	std::string mTextData;
	std::unique_ptr<IDataReader> mReader;
	std::mutex mUpdateLock;
	std::atomic_bool mShutdown;
	int mSD;
};
