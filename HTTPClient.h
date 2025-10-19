#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include <curl/curl.h>

/*
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
*/


namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

namespace SPIDER
{
    /*
	class HTTPClientSinc
	{
	public:
		HTTPClientSinc(std::string host, std::string port = "80", int http_version = 11)
			: host_(std::move(host)), port_(std::move(port)), http_version_(http_version), ioc_(), resolver_(ioc_), stream_(ioc_) {};

		bool get(const std::string& target, std::string& out_body, std::string& out_error);

	private:
		std::string host_;
		std::string port_;
		int http_version_;
		net::io_context ioc_;
		tcp::resolver resolver_;
		beast::tcp_stream stream_;

	};
    */

    class HttpsClient {
    public:
        HttpsClient() {
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();
            if (curl) {
                // Включаем автоматическое следование редиректам
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            }
        }
        ~HttpsClient() {
            if (curl) {
                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();
        }

        // Метод для получения HTML страницы по URL
        std::string get(const std::string& url) {
            if (!curl) {
                throw std::runtime_error("Failed to initialize curl");
            }

            std::string response;

            // Установка URL
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // Включение HTTPS
            curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

            // Обработчик для получения данных
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // (Опционально) проверить SSL-сертификат
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

            // Включение автоматического следования редиректам
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // Выполнение запроса
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                throw std::runtime_error(curl_easy_strerror(res));
            }

            return response;
        }

    private:
        CURL* curl;

        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
            size_t totalSize = size * nmemb;
            output->append((char*)contents, totalSize);
            return totalSize;
        }
    };

}

