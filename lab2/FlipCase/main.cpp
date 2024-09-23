#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <ranges>

// кто такой зомби
// как зомби убить
// как предотвратить появление зомби
// почему много зомби - это плохо

struct Args
{
	std::vector<std::string> files;
};

Args ParseArgs(int argc, char *argv[])
{
	if (argc < 2)
	{
		throw std::invalid_argument("Usage: flip-case <file1> <file2> ...");
	}

	Args args;

	for (int i = 1; i < argc; i++)
	{
		std::string str = argv[i];
		args.files.emplace_back(str);
	}

	return args;
}

char FlipCaseChar(const char& ch)
{
	auto unsignedChar = static_cast<unsigned char>(ch);
	auto result = isupper(unsignedChar) ? tolower(unsignedChar) : toupper(unsignedChar);
	return static_cast<char>(result);
}

void FlipCaseFile(const std::string& inputFileName)
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

	std::cout << "Process " << getpid() << " has finished writing to " << outputFileName << std::endl;
}

void FlipCaseFiles(const std::vector<std::string>& fileNames)
{
	for (const auto& fileName : fileNames)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			throw std::runtime_error("Error forking process");
		}
		if (pid == 0)
		{
			std::cout << "Process " << getpid() << " is processing " << fileName << std::endl;
			FlipCaseFile(fileName);
			// сменить exit
			exit(EXIT_SUCCESS);
		}
	}

	for (int i = 1; i < fileNames.size(); ++i)
	{
		int status;
		pid_t finishedPid = wait(&status);
		std::cout << "Child process " << finishedPid << " is over" << std::endl;
	}
}

int main(int argc, char* argv[])
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
