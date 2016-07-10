#pragma once

#include "LogTargets.h"
#include "ConcurrentQueue.h"
#include <thread>
#include <vector>

class Logger
{
public:
	Logger(LogLevel level);

	Logger(const Logger& o) = delete;

	~Logger();

	Logger& operator=(const Logger& o) = delete;

	void addTarget(ILogTarget::Ptr target);

	void log(LogLevel level, std::string&& message);

private:
	void workerProc();

private:
	std::vector<ILogTarget::Ptr> mTargets;

	struct LogEvent;
	ConcurrentQueue<LogEvent> mLogEvents;

	std::mutex mMutex;
	std::thread mWorker;

	const LogLevel mLevel;
};
