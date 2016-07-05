#pragma once

#include "LogTargets.h"
#include <memory>
#include <mutex>
#include <vector>

class Logger
{
public:
	Logger(LogLevel level);

	Logger(const Logger& o) = delete;

	Logger(LogLevel level, const std::string& filename);

	~Logger() = default;

	Logger& operator=(const Logger& o) = delete;

	void write(LogLevel level, const std::string& msg);

private:
	std::vector<std::unique_ptr<ILogTarget>> mTargets;
	const LogLevel mLevel;
	std::mutex mMutex;
};
