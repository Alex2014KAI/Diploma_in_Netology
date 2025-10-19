#include <iostream>
#include "Indexer.h"
#include "Database.h"
#include "HTTPClient.h"
#include "Spider.h"

#include "Thread_pool.h"
#include "globals.h"

#include "HttpServer.h"
#include "WordSearchEngineDatabase.h"

#include <boost/locale.hpp>

#include <windows.h>
#include <string>
#include <io.h>
#include <fcntl.h>

#include <memory>




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

#ifdef DEBUG_LINK
    SPIDER::Link link("http://lib.ru", 5);
#endif // DEBUG_LINK


#ifdef DEBUG_DATABASE
    
    SPIDER::BDSetup bdSetup("ini.txt");

    SPIDER::Database db(bdSetup.dataSetup_);

    /*
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
    */

    db.deleteTables();
    
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

#endif // DEBUG_HTTPCLIENT

#ifdef DEBUG_INDEXER_AND_DEBUG_HTTPCLIENT
    std::string host = "lib.ru"; // "artofwar.ru";//"lib.ru";
    std::string port = "80";
    std::string target = "/";  // "/b/bobrov/text_1170.shtml"; //"/";
    std::string response_body;
    std::string error;

    SPIDER::BDSetup bdSetup("ini.txt");

    SPIDER::HTTPClientSinc client1(host);
    if (client1.get(target, response_body, error))
    {
        std::cout << "Response body:\n" << response_body << std::endl;
        std::cout << "***********************************************************************" << std::endl;
        SPIDER::Indexer indexer(bdSetup.dataSetup_);
        indexer.execute("lib.ru", response_body);
    }
    else
    {
        std::cerr << "Request failed: " << error << std::endl;
    }
#endif // DEBUG_INDEXER_AND_DEBUG_HTTPCLIENT

#ifdef DEBUG_SPIDER
    SPIDER::SpiderSetup spiderSetup("ini.txt");

    SPIDER::Spider spider(spiderSetup.dataSetupBD_, spiderSetup.startPage_, spiderSetup.depthRecursion_);

    spider.execute();

#endif // DEBUG_SPIDER

#ifdef DEBUG_THREAD_POOL

    SPIDER::Thread_pool thread_pool_;

    //while (true) {
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        SPIDER::Link startLink("lib.ru", 1); // httpbin.org
        thread_pool_.submit(startLink);
        //thread_pool_.submit("http//1");
        std::cout << "**********************************" << std::endl;
        //if (coutnFn == 4) { coutnFn = 0; };
        //coutnFn += 2;
    //}
#endif // DEBUG_THREAD_POOL

#ifdef DEBUG_HTTPS_CLIENT
        try {
            SPIDER::HttpsClient client;
            std::string html;
            html = client.get("https://httpbin.org/");
            std::cout << html << std::endl;
        }
        catch (const std::exception& ex) {
            std::cerr << "Ошибка: " << ex.what() << std::endl;
        }
        return 0;
#endif // DEBUG_HTTPS_CLIENT

#ifdef DEBUG_HTTP_SERVER
        
       try {
           SPIDER::SpiderSetup spiderSetup("ini.txt");
            boost::asio::io_context io_context;

            tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), spiderSetup.portServer_));
            std::cout << "Сервер запущен на порту " << spiderSetup.portServer_ << std::endl;

            while (true) {
                tcp::socket socket(io_context);
                acceptor.accept(socket);
                SPIDER::handle_client_http_(socket);
            }
        }
        catch (std::exception& e) {
            std::cerr << "Исключение: " << e.what() << std::endl;
        }
        return 0;

#endif // DEBUG_HTTP_SERVER

#ifdef DEBUG_WORD_SEARCH_ENGINE_DATA_BASE
        SPIDER::SpiderSetup spiderSetup("ini.txt");

        SPIDER::WordSearchEngineDatabase db(spiderSetup.dataSetupBD_);

        std::vector<std::string> query_words = { "server" };

        auto docs = db.getDocumentsByWords(query_words);

        for (const auto& doc : docs) {
            std::cout << "Document ID: " << doc.document_id
                << ", URL: " << doc.url
                << ", Relevance: " << doc.total_frequency << std::endl;
        }

        return 0;

#endif // DEBUG_WORD_SEARCH_ENGINE_DATA_BASE
        
#ifdef DEBUG_GET_URL
        SPIDER::CleaningHTMLTags debugGet;
        std::string debugHTML1 = "<a href =  #Working_with_EVP_PKEYs ><span class = tocnumber>1< / span> <span class = toctext >Working with EVP_PKEYs< / span>< / a>< / li>";
        std::string debugHTML2 = "<link rel = shortcut icon href = /favicon.ico/ >";

        std::cout << debugGet.getURL(debugHTML1);
        std::cout << debugGet.getURL(debugHTML2);
#endif // DEBUG_GET_URL


#endif // OLD_DEBUG 




#ifndef OLD_DEBUG

        //DELETE
        SPIDER::BDSetup bdSetup("ini.txt");

        SPIDER::Database db(bdSetup.dataSetup_);
        db.deleteTables();
        std::this_thread::sleep_for(1s);
        // DELETE
        // *******************Working code of the program*****************
        //****************************************************************
        try {
        SPIDER::SpiderSetup setupData("ini.txt"); //     https://httpbin.org/ https://wiki.openssl.org
        SPIDER::Link startLink(setupData.startPage_, 1);

        SPIDER::Thread_pool thread_pool_;
        thread_pool_.submit(startLink);

        
        // std::this_thread::sleep_for(40s);

        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), setupData.portServer_));
        std::cout << "Сервер запущен на порту " << setupData.portServer_ << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            SPIDER::handle_client_http_(socket);
            }
        }
        catch (std::exception& e) {
            std::cerr << "Исключение: " << e.what() << std::endl;
        }

        return 0;
#endif // !OLD_DEBUG
        
}