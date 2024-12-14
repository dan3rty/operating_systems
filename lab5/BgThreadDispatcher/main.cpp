#include <iostream>
#include <syncstream>
#include "BgThreadDispatcher.h"

int main()
{
	using osync = std::osyncstream;
	BgThreadDispatcher dispatcher;
	dispatcher.Dispatch([] { osync(std::cout) << "1"; }); // выведет 1 в фоновом потоке
	dispatcher.Dispatch([] { osync(std::cout) << "2"; }); // Выведет 2 в фоновом потоке
	osync(std::cout) << "!"; // Выведет ! в основном потоке
	dispatcher.Wait(); // Дождётся печати 1 и 2
	dispatcher.Dispatch([] { osync(std::cout) << "3"; }); // Выведет 3 в фоновом потоке
    dispatcher.Wait(); // Дождётся вывода 3
}