#include "ServerProcess.h"
#include "Logger.h"
//#include <csignal>
#include <exception>
#include <iostream>
#include <unistd.h>
#include <libconfig.h++>

#define VERSION "0.5"

using namespace std;

static const char* g_configFileName = "/etc/default/I2CSwitchBoard.conf";
static shared_ptr<Logger> g_logger;

/*
static void onShutdownSignal(int value)
{
}

static void setupSignalHandler()
{
	struct sigaction action;
	action.sa_handler = onShutdownSignal;
	action.sa_flags = 0;

	// Block all signals while the handler runs
	sigfillset(&action.sa_mask);

	if (sigaction(SIGINT, &action, nullptr) < 0)
	{
		throw std::runtime_error("Failed to set SIGINT handler.");
	}

	if (sigaction(SIGTERM, &action, nullptr) < 0)
	{
		throw std::runtime_error("Failed to set SIGTERM handler.");
	}
}
*/

static void printHelp()
{
	cout << "Usage: I2CSwitchBoard <Options>" << endl << endl;
	cout << " -c path  Overrides configuration file location." << endl;
	cout << " -h       Displays this help." << endl;
	cout << " -v       Displays version information." << endl;
}

static void printVersion()
{
	cout << "I2CSwitchBoard version " << VERSION << " compiled on " <<
		__DATE__ << " " << __TIME__ << endl;
}

static bool parseArgs(int argc, char* argv[])
{
	int ch;
	while ((ch = getopt(argc, argv, "c:hv")) != -1)
	{
		switch (ch)
		{
		case 'c':
			g_configFileName = optarg;
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

static void loadConfig(ServerProcess::Configuration& cfg)
{
	libconfig::Config cfgfile;
	cfgfile.readFile(g_configFileName);
	const auto& root = cfgfile.getRoot();

	if (!root.lookupValue("network.port", cfg.port))
	{
		cfg.port = 50033;
	}

	if (!root.lookupValue("sensor.poll_interval", cfg.pollInterval))
	{
		cfg.pollInterval = 1000;
	}

	if (!root.lookupValue("sensor.use_dummy_reader", cfg.useDummyReader))
	{
		cfg.useDummyReader = false;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		// TODO Init logger based on config
		g_logger = make_shared<Logger>(LogLevel::TRACE);
	}
	catch (const exception& ex)
	{
		cerr << "Failed to init logger: " << ex.what() << endl;
		return EXIT_FAILURE;
	}

	try
	{
		if (!parseArgs(argc, argv))
		{
			return EXIT_FAILURE;
		}

		ServerProcess::Configuration cfg;
		loadConfig(cfg);

		//setupSignalHandler();
		ServerProcess proc(cfg, g_logger);
		proc.run();

		return EXIT_SUCCESS;
	}
	catch (const exception& ex)
	{
		g_logger->write(LogLevel::ERROR, ex.what());
		return EXIT_FAILURE;
	}
}
