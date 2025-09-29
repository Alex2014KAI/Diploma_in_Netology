#pragma once
#include <map>
#include <iostream>
#include <regex>
#include <fstream>
#include <boost/locale.hpp>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "Database.h"

namespace SPIDER
{
	struct Link {
		std::string url_;
		std::string host_;
		std::string target_;
		int currentRecursionLevel_;
		Link(std::string url, int currentRecursionLevel): url_(url), currentRecursionLevel_(currentRecursionLevel){
			size_t scheme_end = url_.find("://");
			size_t host_start = (scheme_end == std::string::npos) ? 0 : scheme_end + 3;

			size_t path_start = url_.find('/', host_start);

			if (path_start == std::string::npos) {
				host_ = url.substr(host_start);
				target_ = "/";
			}
			else {
				host_ = url.substr(host_start, path_start - host_start);
				target_ = url.substr(path_start);
			}
			std::cout << "Host: " << host_ << std::endl;
			std::cout << "Target: " << target_ << std::endl;
		}
	};

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
		Indexer(): database_("host=localhost port = 5432 dbname = postgres user = postgres password = 738109lexa"), currentLink("", -1) {
			database_.createTables();
		};
		Indexer(const std::string& setupDB) : database_(setupDB), currentLink("", -1) {
			database_.createTables();
		};
		Indexer(const std::string& setupDB, int maxRecursionLevel) : database_(setupDB), maxRecursionLevel_(maxRecursionLevel), currentLink("", -1) {
			database_.createTables();
		};

		void execute(const std::string& URL, const std::string& html);
		void execute(const Link& link, const std::string& html);

		std::vector<std::string> getLinksOnTheCurrentSite();
		std::vector<Link> getLinksOnTheCurrentSiteLink();
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
		std::vector<Link> linksOnTheCurrentSiteLink;
		Link currentLink;

		Database database_;
		int maxRecursionLevel_;

		void pageRequestHTML(const std::string& HTML);
		void pageRequestHTML_Links(const std::string& HTML);
		void convertWordsLowerCase();
		void wordFrequencyAnalysisText();
		void saveDataDatabase();
		void setData(); // Create a table and describe the method for storing data
	};



}
