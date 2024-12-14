#include "Time.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stop_token>
#include <string>
#include <syncstream>
#include <thread>

void Tic(const std::stop_token& stopToken, std::osyncstream& out)
{
	while (!stopToken.stop_requested())
	{
		out << CurrentTimeToString() << " Tic" << std::endl;
		out.emit();
		std::this_thread::sleep_for(2s);
	}
	out << CurrentTimeToString() << " Tic thread completed" << std::endl;
}

void Tac(const std::stop_token& stopToken, std::osyncstream& out)
{
	while (!stopToken.stop_requested())
	{
		out << CurrentTimeToString() << " Tac" << std::endl;
		out.emit();
		std::this_thread::sleep_for(3s);
	}
	out << CurrentTimeToString() << " Tac thread completed" << std::endl;
}

// Упростить программу, заиспользовал stop_source из самих jthread (исправлено)
int main()
{
	std::osyncstream syncOut(std::cout);
	std::jthread ticThread(Tic, std::ref(syncOut));
	std::jthread tacThread(Tac, std::ref(syncOut));

	std::string input;
	std::getline(std::cin, input);

	return EXIT_SUCCESS;
}