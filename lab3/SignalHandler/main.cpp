#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
// что если обработать sigkill не разрушая компьютер
// как пререхватывать исключения колбеков
// std::exception_ptr

std::atomic_flag exitRequested = ATOMIC_FLAG_INIT;

void SignalHandler(const int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		std::cout << "Process " << getpid() << " has received signal #" << signal << std::endl;
		exitRequested.test_and_set();
	}
}

int main()
{
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);

	std::ofstream tempFile("temp.txt");
	if (!tempFile)
	{
		std::cerr << "Error creating temp.txt" << std::endl;
		return EXIT_FAILURE;
	}
	tempFile.close();

	for (int i = 0; i <= 100; ++i)
	{
		if (exitRequested.test())
		{
			break;
		}
		std::cout << i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::filesystem::remove("temp.txt");
	std::cout << "Done" << std::endl;

	return EXIT_SUCCESS;
}