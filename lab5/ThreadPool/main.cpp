#include <iostream>
#include <syncstream>
#include "ThreadPool.h"

int main()
{
	using osync = std::osyncstream;
	ThreadPool pool(2); // Создаем пул из 2 потоков

	pool.Dispatch([] { osync(std::cout) << "A"; }); // Выводит A в фоновом потоке
	pool.Dispatch([] { osync(std::cout) << "B"; }); // Выводит B в фоновом потоке
	osync(std::cout) << "!"; // Выводит ! в основном потоке

	pool.Wait(); // Ждем завершения задач A и B

	pool.Dispatch([] { osync(std::cout) << "C"; }); // Выводит C в фоновом потоке
	pool.Wait(); // Ждем завершения задачи C

	return EXIT_SUCCESS;
}