#pragma once
#include <map>
#include <iostream>
#include <regex>
#include <fstream>
#include <boost/locale.hpp>
#include <vector>

namespace SPIDER
{

	class CleaningHTMLTags {
	public:
		CleaningHTMLTags() : tags("<\\s*\\/?\\s*[a-zA-Z][a-zA-Z0-9]*"
			"(?:\\s+[a-zA-Z_-]+(?:\\s*=\\s*(?:\"[^\"]*\"|'[^']*'|[^>\\s]+))?)*\\s*\\/?\\s*>",
			std::regex::icase),
			non_words("[^\\w]+"),
			multi_spaces("\\s+")
		{
		};
		std::string execute(const std::string& input);
	private:

		std::regex tags;
		std::regex non_words;
		std::regex multi_spaces;

		std::string remove_html_tags(const std::string& input);

		std::string remove_punctuation_and_symbols(const std::string& input);

		std::string normalize_spaces(const std::string& input);


	};


	class Indexer
	{
	public:
		Indexer() = default;
		void execute(const std::string& URL);
		std::vector<std::string> getLinksOnTheCurrentSite(); // !!!!

		private:
		std::map<std::string, int> wordFrequency_;
		std::string originPageHTML_;
		std::string formattedPageHTML_;
		std::string html_;
		CleaningHTMLTags cleaningHTMLTags_;

		std::vector<std::string> linksOnTheCurrentSite;   // !!!!

		void pageRequestHTML(const std::string& URL);
		void convertWordsLowerCase();
		void wordFrequencyAnalysisText();
		void saveDataDatabase();
		void setData(); // Create a table and describe the method for storing data
	};



}
