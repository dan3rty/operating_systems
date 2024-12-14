#pragma once
#include <string>
#include <unordered_map>

struct Document
{
	uint64_t id;
	std::string path;
	std::unordered_map<std::string, int> word_count;
};
