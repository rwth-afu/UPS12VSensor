#pragma once

#include <ctime>
#include <fstream>
#include <memory>
#include <string>

enum class LogLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR
};

std::ostream& operator<<(std::ostream& os, LogLevel level);

class ILogTarget
{
public:
	using Ptr = std::unique_ptr<ILogTarget>;

public:
	virtual ~ILogTarget() = default;

	virtual void write(const std::time_t& ts, LogLevel,
		const std::string& message) = 0;
};

class ConsoleLogTarget : public ILogTarget
{
public:
	virtual void write(const std::time_t& ts, LogLevel level,
		const std::string& message) override;
};

class FileLogTarget : public ILogTarget
{
public:
	FileLogTarget(const std::string& filename);

	FileLogTarget(const FileLogTarget& o) = delete;

	virtual ~FileLogTarget() = default;

	FileLogTarget& operator=(const FileLogTarget& o) = delete;

	virtual void write(const std::time_t& ts, LogLevel level,
		const std::string& message) override;

private:
	std::ofstream mLog;
};
