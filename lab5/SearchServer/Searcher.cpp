#include "Searcher.h"

#include "Indexer.h"

#include <algorithm>
#include <sstream>

std::vector<Document> search_documents(const std::string& query) {
	std::vector<Document> results;
	std::istringstream iss(query);
	std::string word;

	while (iss >> word) {
		std::ranges::transform(word, word.begin(), ::tolower);

		for (const auto& doc : documents) {
			if (doc.word_count.contains(word)  ) {
				results.push_back(doc);
				break;
			}
		}
	}

	return results;
}