#include "FileSystemMonitor.h"

std::string ParseArgs(const int argc, char** argv)
{
	if (argc != 2)
	{
		throw std::invalid_argument("Usage: ./fsmon <path>");
	}

	return argv[1];
}

int main(const int argc, char** argv)
{
	try
	{
		const std::string path = ParseArgs(argc, argv);
		const FileSystemMonitor monitor(path);
		monitor.StartMonitoring();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}