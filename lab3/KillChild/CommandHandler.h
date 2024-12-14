#pragma once
#include "ChildKiller.h"

#include <string>

class CommandHandler
{
public:
	bool HandleCommand(const std::string& command);
	void HandleCommands(std::istream& input);

private:
	ChildKiller ivanTheTerrible;
};
