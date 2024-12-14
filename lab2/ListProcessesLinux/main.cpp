#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void PrintStatusFile(int pid)
{
	std::string statusFile = "/proc/" + std::to_string(pid) + "/status";
	std::ifstream status(statusFile);

	if (!status.is_open())
	{
		std::cerr << "Failed to open: " << statusFile << std::endl;
		return;
	}

	std::string line;
	std::string name;
	std::string vmSize;

	while (std::getline(status, line))
	{
		// доработать чтобы не было проблем с программой с названием VmSize
		if (line.find("Name:", 0) == 0)
		{
			name = line.substr(line.find('\t') + 1);
		}
		if (line.find("VmSize:", 0) == 0)
		{
			vmSize = line.substr(line.find('\t') + 1);
		}
	}

	std::cout << "PID: " << pid << ", Name: " << name << ", Memory: " << vmSize << std::endl;
}

void ListProcesses()
{
	// raii obertka nada
	DIR* dir = opendir("/proc");
	if (dir == nullptr)
	{
		throw std::runtime_error("Failed to open /proc");
	}

	// как освободить от readdir
	dirent* entry;
	while ((entry = readdir(dir)) != nullptr)
	{
		std::string pidStr(entry->d_name);
		if (entry->d_type == DT_DIR && std::ranges::all_of(pidStr, isdigit))
		{
			const int pid = std::stoi(pidStr);
			PrintStatusFile(pid);
		}
	}

	closedir(dir);
}

int main()
{
	try
	{
		ListProcesses();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}