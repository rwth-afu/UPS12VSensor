#include "Logger.h"
#include "ServerProcess.h"
#include "DummyDataReader.h"
#include "I2CDataReader.h"
#include <csignal>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <libconfig.h++>

using namespace std;

struct Configuration
{
	string logFile;
	LogLevel logLevel;
	int port;
	bool useDummy;
	bool useLogFile;
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
	cout << "Usage: I2CSwitchBoard [Options]" << endl << endl;
	cout << "Options:" << endl;
	cout << " -c --config  filename  Specify path to configuration file." <<
		endl;
	cout << " -h --help              Show this help text." << endl;
	cout << " -v --version           Print version information." << endl;
	cout << endl;
}

static void signalHandler(int sig)
{
	switch (sig)
	{
	case SIGHUP:
	case SIGTERM:
	case SIGINT:
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
	static option opts[] =
	{
		{"config", required_argument, nullptr, 'c'},
		{"help", no_argument, nullptr, 'h'},
		{"version", no_argument, nullptr, 'v'},
		{nullptr, 0, nullptr, 0}
	};

	int ch;
	while ((ch = getopt_long(argc, argv, "c:dhv", opts, nullptr)) != -1)
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

	int level;
	if (root.lookupValue("log_level", level))
	{
		switch (level)
		{
		case 0:
			cfg.logLevel = LogLevel::DEBUG;
			break;
		case 1:
			cfg.logLevel = LogLevel::INFO;
			break;
		case 2:
			cfg.logLevel = LogLevel::WARN;
			break;
		case 3:
			cfg.logLevel = LogLevel::ERROR;
			break;
		default:
			cfg.logLevel = LogLevel::INFO;
		}
	}
	else
	{
		cfg.logLevel = LogLevel::INFO;
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
	shared_ptr<Logger> logger;

	try
	{
		logger = make_shared<Logger>(LogLevel::DEBUG);
		logger->addTarget(ILogTarget::Ptr(new ConsoleLogTarget()));

		if (!parseArgs(argc, argv))
		{
			return EXIT_FAILURE;
		}

		const auto cfg = readConfig();

		if (cfg.useLogFile)
		{
			logger->addTarget(ILogTarget::Ptr(
				new FileLogTarget(cfg.logFile)));
		}

		unique_ptr<IDataReader> reader;
		if (cfg.useDummy)
		{
			logger->log(LogLevel::DEBUG, "Creating dummy data reader.");
			reader.reset(new DummyDataReader());
		}
		else
		{
			logger->log(LogLevel::DEBUG, "Creating I2C data reader.");
			reader.reset(new I2CDataReader());
		}

		process.reset(new ServerProcess(logger, move(reader)));

		setupSignalHandlers();

		process->run(cfg.port);
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

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
