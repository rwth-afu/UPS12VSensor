#include "ServerProcess.h"
#include "Logger.h"
//#include <csignal>
#include <exception>
#include <iostream>
#include <unistd.h>

#define VERSION 0.1

using namespace std;

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
	cout << " -d       Use dummy data reader instead of I2C reader." << endl;
	cout << " -h       Displays this help." << endl;
	cout << " -i <int> Sets the polling interval in milliseconds." << endl;
	cout << " -p <int> Sets the TCP port to listen on." << endl;
	cout << " -v       Displays version information." << endl;
}

static void printVersion()
{
	cout << "I2CSwitchBoard version " << VERSION << " compiled on " <<
		__DATE__ << " " << __TIME__ << endl;
}

static bool parseArgs(int argc, char* argv[], ServerProcess::Configuration& cfg)
{
	// Set defaults
	cfg.port = 50033;
	cfg.pollInterval = 1000;
	cfg.useDummyReader = false;

	int ch;
	while ((ch = getopt(argc, argv, "dhi:p:qv")) != -1)
	{
		switch (ch)
		{
		case 'd':
			cfg.useDummyReader = true;
			break;
		case 'h':
			printHelp();
			return false;
		case 'i':
			cfg.pollInterval = stoi(optarg);
			break;
		case 'p':
			cfg.port = stoi(optarg);
			break;
		case 'v':
			printVersion();
			return false;
		default:
			g_logger->write(LogLevel::ERROR, "Invalid command line option.");
			return false;
		}
	}

	return true;
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
		ServerProcess::Configuration cfg;
		if (!parseArgs(argc, argv, cfg))
		{
			return EXIT_FAILURE;
		}

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
