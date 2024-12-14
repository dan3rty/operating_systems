#include <iostream>
#include <thread>
#include "ThreadsafeSet.h"
#include "Time.h"

bool IsPrime(const uint64_t num)
{
	if (num < 2)
	{
		return false;
	}
	if (num <= 3)
	{
		return true;
	}
	for (uint64_t i = 5; i * i <= num; ++i)
	{
		if (num % i == 0)
		{
			return false;
		}
	}
	return true;
}

void FindPrimesInRange(const uint64_t start, const uint64_t end, ThreadSafeSet<uint64_t>& primeSet)
{
	for (uint64_t num = start; num <= end; ++num)
	{
		if (IsPrime(num))
		{
			primeSet.Insert(num);
		}
	}
}

int main()
{
	uint64_t N;
	std::cout << "Enter a number: ";
	std::cin >> N;

	// Single-threaded
	const auto startSingle = std::chrono::high_resolution_clock::now();
	std::unordered_set<uint64_t> singleThreadedPrimes;

	for (uint64_t num = 0; num <= N; ++num)
	{
		if (IsPrime(num))
		{
			singleThreadedPrimes.insert(num);
		}
	}

	const auto endSingle = std::chrono::high_resolution_clock::now();
	const auto durationSingle = std::chrono::duration_cast<std::chrono::milliseconds>(endSingle - startSingle).count();

	// Multithreaded
	ThreadSafeSet<uint64_t> multiThreadedPrimes;
	const auto startMulti = std::chrono::high_resolution_clock::now();

	constexpr int numThreads = 4;
	std::vector<std::thread> threads;

	const uint64_t rangeSize = N / numThreads;

	for (int i = 0; i < numThreads; ++i)
	{
		uint64_t start = i * rangeSize;
		uint64_t end = (i == numThreads - 1) ? N : (start + rangeSize - 1);
		threads.emplace_back(FindPrimesInRange, start, end, std::ref(multiThreadedPrimes));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	const auto end_multi = std::chrono::high_resolution_clock::now();
	const auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - startMulti).count();

	// Output results
	std::cout << "Single-threaded primes: ";

	int count = 0;
	for (const auto& prime : singleThreadedPrimes)
	{
		if (count++ < 10)
		{ // Limit output to 10 primes
			std::cout << prime << " ";
		}
	}

	std::cout << std::endl
			  << "Time taken (single-threaded): " << durationSingle << " ms" << std::endl;

	std::cout << "Multi-threaded primes: ";

	count = 0;
	for (const auto& prime : multiThreadedPrimes.GetElements())
	{
		if (count++ < 10)
		{ // Limit output to 10 primes
			std::cout << prime << " ";
		}
	}

	std::cout << std::endl
			  << "Time taken (multi-threaded): " << duration_multi << " ms" << std::endl;

	return EXIT_SUCCESS;
}