#pragma once

#include <atomic>
#include <memory>
#include <mutex>

class IDataReader;
struct epoll_event;

class ServerProcess
{
public:
	struct Configuration
	{
		int port;
	};

public:
	ServerProcess(const Configuration& cfg, std::unique_ptr<IDataReader> reader);

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
	std::mutex mLock;
	std::atomic_bool mShutdown;
	int mSD;
};
