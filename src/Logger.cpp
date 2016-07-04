#include "Logger.h"

using namespace std;

Logger::Logger(LogLevel level) :
	mLevel(level)
{
	mTargets.emplace_back(new ConsoleLogTarget());
}

Logger::Logger(LogLevel level, const string& filename) :
	Logger(level)
{
	mTargets.emplace_back(new FileLogTarget(filename));
}

void Logger::write(LogLevel level, const string& msg)
{
	lock_guard<mutex> lock(mMutex);

	if (mLevel <= level)
	{
		for (const auto& target : mTargets)
		{
			target->write(level, msg);
		}
	}
}
