#pragma once
#include "Indexer.h"
#include "Database.h"
#include "HTTPClient.h"
#include <thread>

namespace SPIDER
{
	struct SpiderSetup {
		SpiderSetup(const std::string& iniFile);
		std::string dataSetupBD_;
		std::string startPage_;
		int  depthRecursion_;
		int portServer_;

	private:
		std::string host_;
		std::string port_;
		std::string dbname_;
		std::string user_;
		std::string password_;
	};



	class Spider
	{
	public:
		Spider(const std::string& setupDB, int maxRecursionLevel) :
			indexer_(setupDB, maxRecursionLevel) {};
		void execute(const Link link);
		std::vector<std::string> getLinksOnTheCurrentSiteSpider();
		std::vector<Link> getLinksOnTheCurrentSiteSpider_Link();

	private:

		Indexer indexer_;
	};
}

