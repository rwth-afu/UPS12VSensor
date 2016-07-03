#ifdef USE_DUMMY_READER
#include "DummyReader.h"
#else
#include "I2CDataReader.h"
#endif

#include "ServerProcess.h"
//#include <csignal>
#include <exception>
#include <iostream>
#include <unistd.h>

#define VERSION 0.1

using namespace std;

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
	cout << " -h       Displays this help." << endl;
	cout << " -d       Use dummy data reader instead of I2C reader." << endl;
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
	while ((ch = getopt(argc, argv, "hvdp:i:")) != -1)
	{
		switch (ch)
		{
		case 'h':
			printHelp();
			return false;
		case 'v':
			printVersion();
			return false;
		case 'd':
			cfg.useDummyReader = true;
			break;
		case 'p':
			cfg.port = stoi(optarg);
			break;
		case 'i':
			cfg.pollInterval = stoi(optarg);
		default:
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	try
	{
		ServerProcess::Configuration cfg;
		if (!parseArgs(argc, argv, cfg))
		{
			return EXIT_FAILURE;
		}

		//setupSignalHandler();
		ServerProcess proc(cfg);
		proc.run();

		return EXIT_SUCCESS;
	}
	catch (const exception& ex)
	{
		cerr << "Fatal: " << ex.what() << endl;
		return EXIT_FAILURE;
	}
}
