#define CATCH_CONFIG_MAIN
#include "../BgThreadDispatcher.h"
#include "catch.hpp"
#include <chrono>
#include <syncstream>
#include <thread>

TEST_CASE("BgThreadDispatcher with multiple tasks")
{
	BgThreadDispatcher dispatcher;

	bool task1Done = false, task2Done = false, task3Done = false;

	dispatcher.Dispatch([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		task1Done = true;
	});
	dispatcher.Dispatch([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		task2Done = true;
	});
	dispatcher.Dispatch([&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		task3Done = true;
	});

	dispatcher.Wait();

	REQUIRE(task1Done);
	REQUIRE(task2Done);
	REQUIRE(task3Done);
}

TEST_CASE("probability printing exclamation sign")
{
	using osync = std::osyncstream;
	std::stringstream output;

	BgThreadDispatcher dispatcher;
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	osync(output) << "!";
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::string result = output.str();
	REQUIRE(result == "!1" || result == "1!" || result == "!");
}

TEST_CASE("Test BgThreadDispatcher Stop method")
{
	BgThreadDispatcher dispatcher;

	bool taskExecuted = false;

	dispatcher.Dispatch([&] {
		taskExecuted = true;
	});

	dispatcher.Stop();

	dispatcher.Dispatch([&] {
		taskExecuted = false;
	});

	dispatcher.Wait();

	REQUIRE(taskExecuted);
}