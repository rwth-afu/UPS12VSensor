#include "LogTargets.h"
#include <iostream>

using namespace std;

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
		return "??";
	}
}

ostream& operator<<(ostream& os, LogLevel level)
{
	switch (level)
	{
	case LogLevel::DEBUG:
		os << "DEBUG";
		break;
	case LogLevel::INFO:
		os << "INFO";
		break;
	case LogLevel::WARN:
		os << "WARN";
		break;
	case LogLevel::ERROR:
		os << "ERROR";
		break;
	default:
		os << "UNKNOWN";
		break;
	}

	return os;
}

void ConsoleLogTarget::write(const time_t& ts, LogLevel level,
	const string& message)
{
	switch (level)
	{
	case LogLevel::DEBUG:
	case LogLevel::INFO:
		cout << toLocalTime(ts) << " [" << level << "] " << message <<
			endl;
		break;
	default:
		cerr << toLocalTime(ts) << " [" << level << "] " << message <<
			endl;
	}
}

FileLogTarget::FileLogTarget(const string& filename) :
	mLog(filename, ofstream::out | ofstream::app)
{
}

void FileLogTarget::write(const time_t& ts, LogLevel level,
	const string& message)
{
	mLog << toLocalTime(ts) << " [" << level << "] " << message << endl;
}
