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
	// mLevel is const, no need for a lock here
	if (mLevel <= level)
	{
		lock_guard<mutex> lock(mMutex);
		const auto ts = time(nullptr);

		for (const auto& target : mTargets)
		{
			target->write(ts, level, msg);
		}
	}
}
