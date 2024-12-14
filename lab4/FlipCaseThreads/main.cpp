#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

struct Args
{
	std::vector<std::string> files;
};

Args ParseArgs(const int argc, char* argv[])
{
	if (argc < 2)
	{
		throw std::invalid_argument("Usage: flip-case-threads <file1> <file2> ...");
	}

	Args args;

	for (int i = 1; i < argc; i++)
	{
		std::string str = argv[i];
		args.files.emplace_back(str);
	}

	return args;
}

char FlipCaseChar(unsigned char ch)
{
	const auto result = isupper(ch) ? tolower(ch) : toupper(ch);
	return static_cast<char>(result);
}

void FlipCaseFile(std::string const& inputFileName)
{
	std::string outputFileName = inputFileName + ".out";

	std::ifstream inputFile(inputFileName);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("Could not open input file " + inputFileName);
	}

	std::ofstream outputFile(outputFileName);
	if (!outputFile.is_open())
	{
		throw std::runtime_error("Could not open output file " + outputFileName);
	}

	std::string line;
	while (std::getline(inputFile, line))
	{
		std::ranges::transform(line, line.begin(), FlipCaseChar);
		outputFile << line << std::endl;
	}
	if (!inputFile.eof())
	{
		throw std::runtime_error("Error reading input file");
	}

	if (!outputFile.flush())
	{
		throw std::runtime_error("Failed to save data on disk");
	}
}

void FlipCaseFiles(const std::vector<std::string>& fileNames)
{
	std::vector<std::jthread> threads;
	threads.reserve(fileNames.size());
	for (const auto& fileName : fileNames)
	{
		threads.emplace_back(FlipCaseFile, fileName);
	}
}

int main(const int argc, char* argv[])
{
	try
	{
		auto [files] = ParseArgs(argc, argv);
		FlipCaseFiles(files);
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}