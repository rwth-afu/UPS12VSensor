#pragma once

#include "LogTargets.h"
#include <atomic>
#include <mutex>
#include <vector>

class Logger
{
public:
	Logger(LogLevel level);

	Logger(const Logger& o) = delete;

	~Logger() = default;

	Logger& operator=(const Logger& o) = delete;

	void addTarget(ILogTarget::Ptr target);

	void setLogLevel(LogLevel level);

	LogLevel getLogLevel() const;

	void log(LogLevel level, const std::string& message);

private:
	std::atomic<LogLevel> mLevel;
	std::vector<ILogTarget::Ptr> mTargets;
	std::mutex mMutex;
};
