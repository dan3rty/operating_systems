#include "Child.h"

#include <csignal>
#include <iostream>

void Child::Live()
{
	signal(SIGTERM, SignalHandler);
	signal(SIGINT, SignalHandler);

	while (true)
	{
		std::cout << "Child process ID " << getpid() << " is still alive" << std::endl;
		sleep(5);
	}
}

void Child::SignalHandler(const int signum)
{
	if (signum == SIGTERM || signum == SIGINT)
	{
		std::cout << "Child process ID " << getpid() << " received signal " << signum << ", exiting..." << std::endl;
		exit(0);
	}
}
