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

		/*
		std::cout << dataSetupBD_ <<std::endl;
		std::cout << startPage_ << std::endl;
		std::cout << depthRecursion_ << std::endl;
		*/
	}

	void Spider::execute()
	{
		// Возможно в execute нужно единыжды обработать стартовую страничку а далее рекрсией или for проработать глубину сайта
		HTTPClientSinc hTTPClient_(startPage_);
		std::string target = "/";  // Необходимо обрадотать url и выделить host И target
		std::string response_body;
		std::string error;
		if (hTTPClient_.get(target, response_body, error))
		{
			std::cout << "Response body:\n" << response_body << std::endl;
			std::cout << "***********************************************************************" << std::endl;
			indexer_.execute(startPage_, response_body);
		}
		else
		{
			std::cerr << "Request failed: " << error << std::endl;
		}

		return;
	}

	
}