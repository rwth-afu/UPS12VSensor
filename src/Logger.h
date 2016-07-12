#pragma once

#include "LogTargets.h"
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

	void log(LogLevel level, const std::string& message);

private:
	const LogLevel mLevel;
	std::vector<ILogTarget::Ptr> mTargets;
	std::mutex mMutex;
};
