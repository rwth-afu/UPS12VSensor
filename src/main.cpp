//#include "DummySensor.h"
#include "I2CDataReader.h"
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

	int ch;
	while ((ch = getopt(argc, argv, "hvp:")) != -1)
	{
		switch (ch)
		{
		case 'h':
			printHelp();
			return false;
		case 'v':
			printVersion();
			return false;
		case 'p':
			cfg.port = stoi(optarg);
			break;
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
			return false;
		}

		//setupSignalHandler();

		//unique_ptr<IDataReader> reader(new DummySensor());
		unique_ptr<IDataReader> reader(new I2CDataReader());
		ServerProcess proc(cfg, move(reader));
		proc.run();

		return EXIT_SUCCESS;
	}
	catch (const exception& ex)
	{
		cerr << "Fatal: " << ex.what() << endl;
		return EXIT_FAILURE;
	}
}
