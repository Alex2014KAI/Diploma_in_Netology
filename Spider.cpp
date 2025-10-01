#include "Spider.h"

namespace SPIDER
{
	SpiderSetup::SpiderSetup(const std::string& iniFile)
	{
		std::ifstream file(iniFile);
		if (!file) {
			throw std::logic_error("Failed to open file");
			return;
		};

		std::getline(file, host_);
		std::getline(file, port_);
		std::getline(file, dbname_);
		std::getline(file, user_);
		std::getline(file, password_);
		std::getline(file, startPage_);
		std::string debugDepthRecursion;
		std::getline(file, debugDepthRecursion);
		depthRecursion_ = std::stoi(debugDepthRecursion);
		file.close();
		dataSetupBD_ = host_ + " " + port_ + " " + dbname_ + " " + user_ + " " + password_;
	}



	void Spider::execute(const Link link)
	{
		HTTPClientSinc hTTPClient_(link.host_);
		std::string target = link.target_;
		std::string response_body;
		std::string error;
		if (hTTPClient_.get(target, response_body, error))
		{
#ifdef DEBUG_PRINT_DATA
			std::cout << "Response body:\n" << response_body << std::endl;
			std::cout << "***********************************************************************" << std::endl;
#endif // DEBUG_PRINT_DATA
			
			indexer_.execute(link, response_body);
		}
		else
		{
			std::cerr << "Request failed: " << error << std::endl;
		}

		return;
	}

	std::vector<std::string> Spider::getLinksOnTheCurrentSiteSpider()
	{
		return indexer_.getLinksOnTheCurrentSite();
	}

	std::vector<Link> Spider::getLinksOnTheCurrentSiteSpider_Link()
	{
		return indexer_.getLinksOnTheCurrentSiteLink();
	}

	
}