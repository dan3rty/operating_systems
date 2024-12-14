#include "CommandHandler.h"
#include <iostream>

int main()
{
	CommandHandler handler;
	handler.HandleCommands(std::cin);

	return EXIT_SUCCESS;
}
