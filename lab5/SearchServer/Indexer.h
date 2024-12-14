#pragma once
#include "Document.h"
#include <string>
#include <vector>

void index_file(const std::string& path);
void index_directory(const std::string& dir_path);
void index_directory_parallel(const std::string& dir_path);

extern std::vector<Document> documents;