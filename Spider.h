#pragma once
#include "Indexer.h"
#include "Database.h"
#include "HTTPClient.h"

namespace SPIDER
{
	struct SpiderSetup {
		SpiderSetup(const std::string& iniFile);
		std::string dataSetupBD_;
		std::string startPage_;
		int  depthRecursion_;

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
		Spider(const std::string& setupDB, const std::string& startPage, int depthRecursion): 
			indexer_(setupDB), startPage_(startPage), depthRecursion_(depthRecursion){};
		// bool execute(const std::string& host);
		void execute();
	private:

		Indexer indexer_;
		//HTTPClientSinc hTTPClient_;

		std::string startPage_;
		int  depthRecursion_;
	};
}

