#include "CommandHandler.h"
#include <iostream>

bool CommandHandler::HandleCommand(const std::string& command)
{
	if (command == "exit")
	{
		return false;
	}
	if (command == "spawn")
	{
		ivanTheTerrible.SpawnChild();
	}
	else if (command == "kill")
	{
		ivanTheTerrible.KillChild();
	}
	else if (command == "killall")
	{
		ivanTheTerrible.KillAllChildren();
	}
	else
	{
		std::cout << "Unknown command: " << command << std::endl;
	}

	return true;
}

void CommandHandler::HandleCommands(std::istream& input)
{
	std::string command;
	while (std::getline(input, command))
	{
		if (!HandleCommand(command))
		{
			return;
		}
	}
}
