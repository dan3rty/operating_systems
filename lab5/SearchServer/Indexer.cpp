#include "Indexer.h"
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>

std::vector<Document> documents;
std::mutex doc_mutex;

void index_file(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Ошибка открытия файла: " << path << std::endl;
		return;
	}

	Document doc;
	doc.id = documents.size() + 1; // Автоинкремент ID
	doc.path = path;

	std::string word;
	while (file >> word) {
		// Приводим слово к нижнему регистру
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		doc.word_count[word]++;
	}

	std::lock_guard<std::mutex> lock(doc_mutex);
	documents.push_back(doc); // Добавляем документ в индекс
}

void index_directory(const std::string& dir_path) {
	for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
		if (entry.is_regular_file()) {
			index_file(entry.path().string());
		}
	}
}

void index_directory_parallel(const std::string& dir_path) {
	std::vector<std::future<void>> futures;

	for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
		if (entry.is_regular_file()) {
			futures.emplace_back(std::async(std::launch::async, index_file, entry.path().string()));
		}
	}

	for (auto& future : futures) {
		future.get(); // Ждем завершения всех потоков
	}
}