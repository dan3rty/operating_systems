#include "SnowfallApp.h"
#include <iostream>

constexpr int SNOWFLAKE_COUNT = 1000;

int main()
{
	try
	{
		SnowfallApp app(SNOWFLAKE_COUNT);
		app.Run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
