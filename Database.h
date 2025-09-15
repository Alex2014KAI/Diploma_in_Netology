#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <pqxx/pqxx>

namespace SPIDER
{
	struct BDSetup {
	private:
		std::string host_;
		std::string port_;
		std::string dbname_;
		std::string user_;
		std::string password_;
	public:
		BDSetup(const std::string& iniFile);
		std::string dataSetup_;
		void printDataBase(); // debug metod
	};

	class Database
	{
	public:
		Database(std::string dataSetup);
		void createTables();
		void deleteTables();

	private:
		pqxx::connection c;
	};

}

