#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <pqxx/pqxx>
#include <map>

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
		Database(const std::string& dataSetup);
		void createTables();
		void deleteTables();
		void writeDataToTable(const std::string& url, const std::map<std::string, int>& wordFrequency);
		bool checkingForURLExistence(const std::string& url);

		

	private:
		pqxx::connection c;

		int addURL(const std::string& url);
		int addWord(const std::string& word);
		void insertOrUpdateFrequency(int document_id, int word_id, int frequency);
	};

}

