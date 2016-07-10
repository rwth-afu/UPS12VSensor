#include "Logger.h"
#include <iostream>

using namespace std;

struct Logger::LogEvent
{
	const time_t ts;
	const LogLevel level;
	const string message;

	LogEvent(LogLevel level, const string& message) :
		ts(time(nullptr)),
		level(level),
		message(message)
	{
	}
};

Logger::Logger(LogLevel level) :
	mLevel(level)
{
	// Start worker thread
	mWorker = thread(&Logger::workerProc, this);
}

Logger::~Logger()
{
	try
	{
		if (mWorker.joinable())
		{
			mLogEvents.notifyWaiting();
			mWorker.detach();
		}
	}
	catch (const exception& ex)
	{
		// ignore it :)
	}
}

void Logger::log(LogLevel level, string&& message)
{
	// TODO Check if worker thread is running

	if (mLevel <= level)
	{
		mLogEvents.emplace(level, move(message));
	}
}

void Logger::addTarget(ILogTarget::Ptr target)
{
	lock_guard<mutex> lck(mMutex);
	mTargets.push_back(move(target));
}

void Logger::workerProc()
{
	try
	{
		while (true)
		{
			auto ev = mLogEvents.pop();

			lock_guard<mutex> lck(mMutex);
			for (const auto& target : mTargets)
			{
				target->write(ev.ts, ev.level, ev.message);
			}
		}
	}
	catch (const InterruptedException& ex)
	{
		// Queue received kill event
	}
	catch (const exception& ex)
	{
		cerr << "Logger: " << ex.what() << endl;
		// TODO Stop application?
	}
}
