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

void ConsoleLogTarget::write(LogLevel level, const string& message)
{
	cout << "[" << level << "] " << message << endl;
}

FileLogTarget::FileLogTarget(const string& filename) :
	mOut(filename, ofstream::out | ofstream::app)
{
}

void FileLogTarget::write(LogLevel level, const string& message)
{
	mOut << "[" << level << "] " << message << endl;
}
