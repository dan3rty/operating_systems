#include "Indexer.h"
#include "Searcher.h"
#include <iostream>

int main()
{
	std::string command;

	while (true)
	{
		std::cout << "> ";
		std::getline(std::cin, command);

		if (command.starts_with("add_file "))
		{
			index_file(command.substr(9));
		}
		else if (command.starts_with("add_dir "))
		{
			index_directory(command.substr(8));
		}
		else if (command.starts_with("find "))
		{
			auto results = search_documents(command.substr(5));
			for (const auto& [id, path, word_count] : results)
			{
				std::cout << "Found: " << path << " with ID: " << id << "\n";
			}
		}
		else if (command == "exit")
		{
			break;
		}
		else
		{
			std::cout << "Неизвестная команда\n";
		}
	}

	return 0;
}