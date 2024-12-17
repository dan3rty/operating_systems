#include "FileSystemMonitor.h"

std::string ParseArgs(int argc, char* argv[])
{
	if (argc != 2)
	{
		throw std::invalid_argument("Usage: fsmonw PATH");
	}

	return argv[1];
}

void RunDirectoryMonitor(const std::string& path)
{
	FileSystemMonitor monitor(path);
	monitor.StartMonitoring();
}

int main(int argc, char* argv[])
{
	try
	{
		std::string directoryPath = ParseArgs(argc, argv);
		RunDirectoryMonitor(directoryPath);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
