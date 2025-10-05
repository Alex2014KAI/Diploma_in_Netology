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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

namespace SPIDER
{
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

    
    class HttpsClient {
    public:
    HttpsClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
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

        // Выполнение запроса
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        return response;
    }

private:
    CURL* curl;

    // Статическая функция-обработчик для записи полученных данных
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }
    };
   

    /*
    class HttpsClient {
    public:
        HttpsClient() {
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);

            SSL_load_error_strings();
            OpenSSL_add_ssl_algorithms();

            ctx = SSL_CTX_new(TLS_client_method());
        }

        ~HttpsClient() {
            SSL_CTX_free(ctx);
            EVP_cleanup();
            WSACleanup();
        }

        // Блокирующий вызов: отправляет запрос и ожидает ответ
        std::string get(const std::string& url) {
            // Парсинг URL
            std::string protocol, host, path;
            int port = 443; // по умолчанию HTTPS

            parse_url(url, protocol, host, path, port);
            if (protocol != "https") {
                throw std::runtime_error("Только HTTPS поддерживается");
            }

            // Создание сокета
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock == INVALID_SOCKET) {
                throw std::runtime_error("Не удалось создать сокет");
            }

            // Получение IP-адреса хоста
            struct addrinfo hints = {}, * res = nullptr;
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            std::string host_port = host + ":" + std::to_string(port);
            if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
                closesocket(sock);
                throw std::runtime_error("Не удалось разрешить адрес");
            }

            // Подключение
            if (connect(sock, res->ai_addr, (int)res->ai_addrlen) != 0) {
                freeaddrinfo(res);
                closesocket(sock);
                throw std::runtime_error("Не удалось подключиться");
            }
            freeaddrinfo(res);

            // Инициализация SSL
            SSL* ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);
            if (SSL_connect(ssl) != 1) {
                SSL_free(ssl);
                closesocket(sock);
                throw std::runtime_error("SSL соединение не установлено");
            }

            // Формируем запрос
            std::stringstream ss;
            ss << "GET " << path << " HTTP/1.1\r\n";
            ss << "Host: " << host << "\r\n";
            ss << "Connection: close\r\n\r\n";

            std::string request = ss.str();

            // Отправка запроса
            SSL_write(ssl, request.c_str(), (int)request.size());

            // Чтение ответа
            std::string response;
            const int buf_size = 4096;
            char buf[buf_size];
            int bytes_read;

            while ((bytes_read = SSL_read(ssl, buf, buf_size)) > 0) {
                response.append(buf, bytes_read);
            }

            // Очистка
            SSL_shutdown(ssl);
            SSL_free(ssl);
            closesocket(sock);

            // Удалим HTTP-заголовки, если нужно
            return extract_body(response);
        }

    private:
        SSL_CTX* ctx;

        void parse_url(const std::string& url, std::string& protocol, std::string& host,
            std::string& path, int& port) {
            // Очень простая парсинг
            size_t protocol_end = url.find("://");
            protocol = url.substr(0, protocol_end);
            size_t host_start = protocol_end + 3;
            size_t path_start = url.find('/', host_start);
            if (path_start == std::string::npos) {
                host = url.substr(host_start);
                path = "/";
            }
            else {
                host = url.substr(host_start, path_start - host_start);
                path = url.substr(path_start);
            }

            // Проверка наличия порта
            size_t colon_pos = host.find(':');
            if (colon_pos != std::string::npos) {
                port = std::stoi(host.substr(colon_pos + 1));
                host = host.substr(0, colon_pos);
            }
        }

        std::string extract_body(const std::string& response) {
            size_t header_end = response.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                return response.substr(header_end + 4);
            }
            return response; // или пустая строка
        }
    };
    */

}

