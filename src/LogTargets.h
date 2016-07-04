#pragma once

#include <string>
#include <fstream>

enum class LogLevel
{
	TRACE,
	INFO,
	ERROR
};

std::ostream& operator<<(std::ostream& os, LogLevel level);

class ILogTarget
{
public:
	virtual ~ILogTarget() = default;

	virtual void write(LogLevel level, const std::string& msg) = 0;
};

class ConsoleLogTarget : public ILogTarget
{
public:
	virtual ~ConsoleLogTarget() = default;

	virtual void write(LogLevel level, const std::string& msg) override;
};

class FileLogTarget : public ILogTarget
{
public:
	FileLogTarget(const std::string& filename);

	virtual ~FileLogTarget() = default;

	virtual void write(LogLevel level, const std::string& msg) override;

private:
	std::ofstream mOut;
};
