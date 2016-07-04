#pragma once

#include "LogTargets.h"
#include <memory>
#include <mutex>
#include <vector>

class Logger
{
public:
	Logger(LogLevel level);

	Logger(LogLevel level, const std::string& filename);

	~Logger() = default;

	void write(LogLevel level, const std::string& msg);

private:
	std::vector<std::unique_ptr<ILogTarget>> mTargets;
	const LogLevel mLevel;
	std::mutex mMutex;
};
