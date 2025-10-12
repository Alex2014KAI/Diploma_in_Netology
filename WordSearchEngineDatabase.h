#pragma once

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <iostream>


namespace SPIDER
{
	struct DocumentFrequency {
		int document_id;
		std::string url;
		int total_frequency;
	};

	class WordSearchEngineDatabase
	{
    public:
        WordSearchEngineDatabase(const std::string& conn_str) : conn(conn_str) {};

        std::vector<DocumentFrequency> getDocumentsByWords(const std::vector<std::string>& words);

    private:
        pqxx::connection conn;
    };
}

