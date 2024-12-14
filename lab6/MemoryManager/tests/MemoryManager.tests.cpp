#define CATCH_CONFIG_MAIN
#include "../MemoryManager.h"
#include "catch.hpp"
#include <thread>

TEST_CASE("MemoryManager basic functionality", "[memory_manager]")
{
	alignas(sizeof(std::max_align_t)) char buffer[1000];
	MemoryManager memoryManager(buffer, sizeof(buffer));

	SECTION("Allocate and free memory")
	{
		auto ptr = memoryManager.Allocate(sizeof(double));
		REQUIRE(ptr != nullptr);

		auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
		REQUIRE(*value == 3.1415927);

		memoryManager.Free(ptr);
	}

	SECTION("Allocate multiple blocks")
	{
		auto ptr1 = memoryManager.Allocate(16);
		REQUIRE(ptr1 != nullptr);

		auto ptr2 = memoryManager.Allocate(32);
		REQUIRE(ptr2 != nullptr);

		memoryManager.Free(ptr1);
		memoryManager.Free(ptr2);
	}

	SECTION("Allocate more than available memory")
	{
		auto ptr1 = memoryManager.Allocate(900);
		REQUIRE(ptr1 != nullptr);

		auto ptr2 = memoryManager.Allocate(200);
		REQUIRE(ptr2 == nullptr);

		memoryManager.Free(ptr1);
	}
}

TEST_CASE("MemoryManager merging free blocks", "[memory_manager][merge]")
{
	alignas(sizeof(std::max_align_t)) char buffer[1000];
	MemoryManager memoryManager(buffer, sizeof(buffer));

	void* ptr1 = memoryManager.Allocate(200);
	void* ptr2 = memoryManager.Allocate(200);

	memoryManager.Free(ptr1);
	memoryManager.Free(ptr2);

	void* ptr3 = memoryManager.Allocate(400);
	REQUIRE(ptr3 != nullptr);
}

TEST_CASE("MemoryManager thread safety", "[memory_manager][thread]")
{
	alignas(sizeof(std::max_align_t)) char buffer[1000];
	MemoryManager memoryManager(buffer, sizeof(buffer));

	auto allocateMemory = [&memoryManager]() {
		for (int i = 0; i < 10; ++i)
		{
			void* ptr = memoryManager.Allocate(16);
			REQUIRE(ptr != nullptr);
			memoryManager.Free(ptr);
		}
	};

	std::thread t1(allocateMemory);
	std::thread t2(allocateMemory);

	t1.join();
	t2.join();
}
