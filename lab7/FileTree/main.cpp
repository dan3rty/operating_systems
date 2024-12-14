#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

std::vector<fs::path> GetSortedEntries(const fs::path& path)
{
	std::vector<fs::path> entries;
	for (const auto& entry : fs::directory_iterator(path))
	{
		entries.emplace_back(entry.path());
	}
	std::ranges::sort(entries);
	return entries;
}

void DisplayFile(const fs::path& entry, const std::string& prefix)
{
	std::cout << prefix << "|   "
			  << "[F] " << entry.filename().string() << std::endl;
}

void DisplayDirectory(const fs::path& entry, const std::string& prefix, const bool isLastEntry)
{
	std::cout << prefix << (isLastEntry ? "\\---" : "+---") << "[D] " << entry.filename().string() << std::endl;
}

std::string GetNextPrefix(const std::string& currentPrefix, const bool isLastEntry)
{
	return currentPrefix + (isLastEntry ? "    " : "|   ");
}

void DisplayFileTreeRecursive(const fs::path& path, const std::string& prefix = "", const bool isFirst = true)
{
	if (!exists(path) || !is_directory(path))
	{
		return;
	}

	const auto entries = GetSortedEntries(path);

	for (const auto& entry : entries)
	{
		if (is_regular_file(entry))
		{
			DisplayFile(entry, prefix);
		}
	}

	for (size_t i = 0; i < entries.size(); ++i)
	{
		const auto& entry = entries[i];
		if (is_directory(entry))
		{
			const bool isLastEntry = (i == entries.size() - 1);
			if (!isFirst)
			{
				std::cout << prefix << "|" << std::endl;
			}
			DisplayDirectory(entry, prefix, isLastEntry);
			DisplayFileTreeRecursive(entry, GetNextPrefix(prefix, isLastEntry), false);
		}
	}
}

void DisplayFileTree(const fs::path& path)
{
	if (!exists(path) || !is_directory(path))
	{
		throw std::invalid_argument("Указанный путь не существует или не является директорией");
	}

	std::cout << path.filename().string() << std::endl;
	DisplayFileTreeRecursive(path);
}

int main(int argc, char* argv[])
{
	try
	{
		const fs::path dirPath = (argc > 1) ? argv[1] : fs::current_path();
		DisplayFileTree(dirPath);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}