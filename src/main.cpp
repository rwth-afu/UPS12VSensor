#include "Logger.h"
#include "ServerProcess.h"
#include "DummyDataReader.h"
#include "I2CDataReader.h"
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <libconfig.h++>

using namespace std;

struct Configuration
{
	string logFile;
	int port;
	bool useLogFile;
	bool useDummy;
};

static const char* opt_configFile = "/etc/default/I2CSwitchBoard.conf";
static unique_ptr<ServerProcess> process;

static void printVersion()
{
	cout << "I2CSwitchBoard 0.1 compiled on " << __DATE__ << " " <<
		__TIME__ << endl;
}

static void printHelp()
{
	cout << "Usage: I2CSwitchBoard [-c path] [-h] [-v]" << endl;
	cout << " -c path Specify path to configuration file." << endl;
	cout << " -h      Show this help text." << endl;
	cout << " -v      Print version information." << endl;
}

static void signalHandler(int sig)
{
	switch (sig)
	{
	case SIGTERM:
	case SIGINT:
	case SIGHUP:
		process->stop();
		break;
	}
}

static void setupSignalHandlers()
{
	struct sigaction act;
	act.sa_handler = signalHandler;
	act.sa_flags = 0;
	sigfillset(&act.sa_mask);

	if (sigaction(SIGINT, &act, nullptr) < 0)
	{
		throw runtime_error("Failed to set handler for SIGINT.");
	}

	if (sigaction(SIGTERM, &act, nullptr) < 0)
	{
		throw runtime_error("Failed to set handler for SIGTERM.");
	}

	if (sigaction(SIGHUP, &act, nullptr) < 0)
	{
		throw runtime_error("Failed to set handler for SIGHUP.");
	}
}

static bool parseArgs(int argc, char* argv[])
{
	int ch;
	while ((ch = getopt(argc, argv, "c:hv")) != -1)
	{
		switch (ch)
		{
		case 'c':
			opt_configFile = optarg;
			break;
		case 'h':
			printHelp();
			return false;
		case 'v':
			printVersion();
			return false;
		default:
			return false;
		}
	}

	return true;
}

static Configuration readConfig()
{
	libconfig::Config cfgFile;
	cfgFile.readFile(opt_configFile);

	Configuration cfg;
	const auto& root = cfgFile.getRoot();

	if (!root.lookupValue("port", cfg.port))
	{
		cfg.port = 50033;
	}

	if (!root.lookupValue("use_dummy_reader", cfg.useDummy))
	{
		cfg.useDummy = false;
	}

	if (!root.lookupValue("use_log_file", cfg.useLogFile))
	{
		cfg.useLogFile = false;
	}

	if (!root.lookupValue("log_file_path", cfg.logFile))
	{
		cfg.logFile = "/var/log/I2CSwitchBoard.log";
	}

	return cfg;
}

int main(int argc, char* argv[])
{
	int status = EXIT_SUCCESS;
	shared_ptr<Logger> logger;

	try
	{
		logger = make_shared<Logger>(LogLevel::DEBUG);
		logger->addTarget(ILogTarget::Ptr(new ConsoleLogTarget()));

		if (parseArgs(argc, argv))
		{
			const auto cfg = readConfig();

			if (cfg.useLogFile)
			{
				logger->addTarget(ILogTarget::Ptr(
					new FileLogTarget(cfg.logFile)));
			}

			unique_ptr<IDataReader> reader;
			if (cfg.useDummy)
			{
				reader.reset(new DummyDataReader());
			}
			else
			{
				reader.reset(new I2CDataReader());
			}

			process.reset(new ServerProcess(logger, move(reader)));

			setupSignalHandlers();

			process->run(cfg.port);
		}
		else
		{
			status = EXIT_FAILURE;
		}
	}
	catch (const exception& ex)
	{
		if (logger)
		{
			logger->log(LogLevel::ERROR, ex.what());
		}
		else
		{
			cerr << "Fatal: " << ex.what() << endl;
		}

		status = EXIT_FAILURE;
	}

	// Wait some time so pending log events get processed.
	this_thread::sleep_for(chrono::seconds(1));

	return status;
}
