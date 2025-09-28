#pragma once
#include <map>
#include <iostream>
#include <regex>
#include <fstream>
#include <boost/locale.hpp>
#include <vector>
#include <algorithm>

#include "Database.h"

namespace SPIDER
{

	class CleaningHTMLTags {
	public:
		CleaningHTMLTags() : tags("<\\s*\\/?\\s*[a-zA-Z][a-zA-Z0-9]*"
			"(?:\\s+[a-zA-Z_-]+(?:\\s*=\\s*(?:\"[^\"]*\"|'[^']*'|[^>\\s]+))?)*\\s*\\/?\\s*>",
			std::regex::icase),
			non_words("[^\\w]+"),
			multi_spaces("\\s+"),
			hrefTegs("href=[\"']?([^\"' >#]+)", std::regex::icase){};
		std::string execute(const std::string& input);
		std::string getURL(const std::string& html);
	private:

		std::regex tags;
		std::regex non_words;
		std::regex multi_spaces;
		std::regex hrefTegs;

		std::string remove_html_tags(const std::string& input);

		std::string remove_punctuation_and_symbols(const std::string& input);

		std::string normalize_spaces(const std::string& input);
	};



	class Indexer
	{
	public:
		Indexer(): database_("host=localhost port = 5432 dbname = postgres user = postgres password = 738109lexa") {};
		Indexer(const std::string& setupDB) : database_(setupDB) {};

		void execute(const std::string& URL, const std::string& html);
		std::vector<std::string> getLinksOnTheCurrentSite();
		std::map<std::string, int> getWordFrequency();
		void printDataServer(); // Method for debugging

		private:
		std::map<std::string, int> wordFrequency_;
		std::string originPageHTML_;
		std::string formattedPageHTML_;
		std::string url_;
		std::string html_;
		CleaningHTMLTags cleaningHTMLTags_;

		std::vector<std::string> linksOnTheCurrentSite;

		Database database_;

		void pageRequestHTML(const std::string& HTML);
		void convertWordsLowerCase();
		void wordFrequencyAnalysisText();
		void saveDataDatabase();
		void setData(); // Create a table and describe the method for storing data
	};



}
