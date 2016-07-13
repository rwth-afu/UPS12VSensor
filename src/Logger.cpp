#include "Logger.h"

using namespace std;

Logger::Logger(LogLevel level) :
	mLevel(level)
{
}

void Logger::log(LogLevel level, const string& message)
{
	if (mLevel <= level)
	{
		const auto ts = time(nullptr);

		lock_guard<mutex> lck(mMutex);
		for (const auto& target : mTargets)
		{
			target->write(ts, level, message);
		}
	}
}

void Logger::addTarget(ILogTarget::Ptr target)
{
	lock_guard<mutex> lck(mMutex);
	mTargets.push_back(move(target));
}

void Logger::setLogLevel(LogLevel level)
{
	mLevel = level;
}
