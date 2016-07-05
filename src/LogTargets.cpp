#include "LogTargets.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& os, LogLevel level)
{
	switch (level)
	{
	case LogLevel::TRACE:
		os << "TRACE";
		break;
	case LogLevel::INFO:
		os << "INFO";
		break;
	case LogLevel::ERROR:
		os << "ERROR";
		break;
	default:
		os << "INVALID";
		break;
	}

	return os;
}

static string toLocalTime(const time_t& ts)
{
	const auto ltm = localtime(&ts);
	char buffer[80];
	if (strftime(buffer, sizeof(buffer), "%F %T", ltm) > 0)
	{
		return buffer;
	}
	else
	{
		return string();
	}
}

void ConsoleLogTarget::write(const time_t& ts, LogLevel level,
	const string& message)
{
	// std::put_time from <iomanip> not yet supported by all compilers
	cout << toLocalTime(ts) << " [" << level << "] " << message << endl;
}

FileLogTarget::FileLogTarget(const string& filename) :
	mOut(filename, ofstream::out | ofstream::app)
{
}

void FileLogTarget::write(const time_t& ts, LogLevel level,
	const string& message)
{
	mOut << toLocalTime(ts) << " [" << level << "] " << message << endl;
}
