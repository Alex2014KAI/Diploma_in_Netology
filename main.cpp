#include <iostream>
#include "Indexer.h"
#include "Database.h"
#include "HTTPClient.h"
#include "Spider.h"

#include <boost/locale.hpp>

#include <windows.h>
#include <string>
#include <io.h>
#include <fcntl.h>

#define OLD_DEBUG
//#define DEBUG_INDEXER
//#define DEBUG_DATABASE
//#define DEBUG_HTTPCLIENT
#define DEBUG_INDEXER_AND_DEBUG_HTTPCLIENT
//#define DEBUG_SPIDER


int main()
{

    SetConsoleOutputCP(CP_UTF8);
    //_setmode(_fileno(stdout), _O_U8TEXT);
    setlocale(LC_ALL, "Russian");
    //std::locale::global(std::locale("ru_RU.UTF-8"));
    //std::wcout.imbue(std::locale());

#ifdef OLD_DEBUG 

#ifdef DEBUG_INDEXER
    SPIDER::Indexer indexer;
    indexer.execute("html.txt");
#endif // DEBUG_INDEXER


#ifdef DEBUG_DATABASE
    SPIDER::BDSetup bdSetup("ini.txt");

    SPIDER::Database db(bdSetup.dataSetup_);

    
    db.createTables();
    std::map<std::string, int> debugWordFrequency1 = { 
        {"LExa", 5},
        {"Dima", 4 },
        {"Nina", 10},
        {"Oleg", 8},
        {"Artur", 220}
    };
    db.writeDataToTable("https//dataInfo", debugWordFrequency1);

    std::map<std::string, int> debugWordFrequency2 = {
        {"dom", 5},
        {"krovat", 4 },
        {"mebel", 10},
        {"divan", 8},
        {"stol", 220}
    };
    db.writeDataToTable("https//debugInfo", debugWordFrequency2);
    
    if (db.checkingForURLExistence("https//debugInfo")) {
        std::cout << "https//debugInfo - SUCHECTVUET!!";
    };

    // db.deleteTables();
    
#endif // DEBUG_DATABASE

#ifdef DEBUG_HTTPCLIENT
    
    std::string host = "lib.ru";
    std::string port = "80";
    /*
    80 — HTTP
    443 — HTTPS
    21 — FTP
    22 — SSH
    25 — SMTP
    3306 — MySQL (база данных)
    8080 — часто используется для HTTP-прокси или альтернативных HTTP-серверов
    */

    
    std::string target = "/";
    std::string response_body;
    std::string error;

    SPIDER::HTTPClientSinc client1(host);
    if (client1.get(target, response_body, error))
    {
        std::cout << "Response body:\n" << response_body << std::endl;
    }
    else
    {
        std::cerr << "Request failed: " << error << std::endl;
    }

    /*
    std::cout << "*****************************************************************************************" << std::endl;

    host = "httpbin.org";
    SPIDER::HTTPClientSinc client2(host, port);
    if (client2.get(target, response_body, error))
    {
        std::cout << "Response body:\n" << response_body << std::endl;
    }
    else
    {
        std::cerr << "Request failed: " << error << std::endl;
    }

    */
    
    


    /*
    net::io_context ioc;

    // Хост и путь запроса
    auto client = std::make_shared<SPIDER::HTTPClient>(ioc, "lib.ru", "/");

    client->run();

    ioc.run();
    */
#endif // DEBUG_HTTPCLIENT

#ifdef DEBUG_INDEXER_AND_DEBUG_HTTPCLIENT
    std::string host = "lib.ru"; // "artofwar.ru";//"lib.ru";
    std::string port = "80";
    std::string target = "/";  // "/b/bobrov/text_1170.shtml"; //"/";
    std::string response_body;
    std::string error;

    SPIDER::HTTPClientSinc client1(host);
    if (client1.get(target, response_body, error))
    {
        std::cout << "Response body:\n" << response_body << std::endl;
        std::cout << "***********************************************************************" << std::endl;
        SPIDER::Indexer indexer;
        indexer.execute("lib.ru", response_body);
    }
    else
    {
        std::cerr << "Request failed: " << error << std::endl;
    }
#endif // DEBUG_INDEXER_AND_DEBUG_HTTPCLIENT

#ifdef DEBUG_SPIDER
    SPIDER::SpiderSetup spiderSetup("ini.txt");

#endif // DEBUG_SPIDER


#endif // OLD_DEBUG 

}