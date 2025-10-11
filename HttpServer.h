#pragma once


#include <boost/asio.hpp>
#include <iostream>
#include <string>


#include <vector>
#include <sstream>
#include <algorithm>

// #define WORK_KODE_DEBUG


namespace SPIDER
{
#ifdef WORK_KODE_DEBUG
    using boost::asio::ip::tcp;
    const std::string html_response_template = "<html> <head><title>Ответ</title></head> <body> <h1>Received Word: {word}</h1><p>Number 1: {num1}</p> <p>Number 2: {num2}</p> </body> </html>";

    // Функция для замены плейсхолдеров в шаблоне
    inline std::string replace_all_(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    inline void handle_client_http_(tcp::socket& socket) {
        try {
            boost::asio::streambuf request_buf;
            boost::system::error_code error;

            // Читаем заголовки
            size_t len = boost::asio::read_until(socket, request_buf, "\r\n\r\n", error);
            if (error && error != boost::asio::error::eof) {
                std::cerr << "Ошибка чтения заголовков: " << error.message() << std::endl;
                return;
            }

            std::istream request_stream(&request_buf);
            std::string request_line;
            std::getline(request_stream, request_line);
            std::string method, path, version;

            std::istringstream req_line_stream(request_line);
            req_line_stream >> method >> path >> version;

            // Читаем остальные заголовки
            std::string header;
            size_t content_length = 0;
            while (std::getline(request_stream, header) && header != "\r") {
                if (header.find("Content-Length:") != std::string::npos) {
                    // Получаем длину тела
                    std::string cl = header.substr(header.find(":") + 1);
                    content_length = std::stoi(cl);
                }
            }

            std::string body;

            if (content_length > 0) {
                // читаем тело
                if (request_buf.size() < content_length) {
                    boost::asio::read(socket, request_buf, boost::asio::transfer_exactly(content_length - request_buf.size()), error);
                }
                std::istream body_stream(&request_buf);
                std::vector<char> buf(content_length);
                body_stream.read(buf.data(), content_length);
                body.assign(buf.begin(), buf.end());
            }

            // Распарсим POST-данные (например, application/x-www-form-urlencoded: слово=Значение)
            std::string word;
            auto pos = body.find("word=");
            if (pos != std::string::npos) {
                auto end_pos = body.find('&', pos);
                word = body.substr(pos + 5, end_pos - (pos + 5));
            }
            std::cout << word;

            // Тестовая логика: возвращаем два числа
            // тут нужно возвращать число повоторений слов в БД
            // WordSearchEngineDatabase wordSearchEngineDatabase(word, count);
            // word - слово
            // count - число повторений
            int num1 = 42;
            int num2 = 24;

            // Готовим HTML-ответ
            std::string html_response = replace_all_(html_response_template, "{word}", word);
            html_response = replace_all_(html_response, "{num1}", std::to_string(num1));
            html_response = replace_all_(html_response, "{num2}", std::to_string(num2));

            // Формируем ответ
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Access-Control-Allow-Origin: *\r\n" // разрешение CORS для всех доменов
                "Content-Length: " + std::to_string(html_response.size()) + "\r\n"
                "\r\n" + html_response;

            boost::asio::write(socket, boost::asio::buffer(response), error);

        }
        catch (std::exception& e) {
            std::cerr << "Исключение: " << e.what() << std::endl;
        }
    }
#endif // WORK_KODE_DEBUG
    inline void splitByDelimiter(const std::string& input, const std::string& delimiter, std::vector<std::string>& words) {
        size_t start = 0;
        size_t end;

        while ((end = input.find(delimiter, start)) != std::string::npos) {
            // извлекаем слово между start и end
            std::string word = input.substr(start, end - start);
            words.push_back(word);
            start = end + delimiter.length(); // сдвигаемся за разделитель
        }
        // добавляем последний фрагмент, если он есть
        if (start < input.size()) {
            words.push_back(input.substr(start));
        }
    }

    using boost::asio::ip::tcp;
    const std::string html_response_template = "<html> <head><title>Ответ</title></head> <body> <h1>The word/combination of words entered: {word}</h1><p> Number of repetitions on sites: {num1}</p> </body> </html>";
    // Функция для замены плейсхолдеров в шаблоне
    inline std::string replace_all_(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    inline void handle_client_http_(tcp::socket& socket) {
        try {
            boost::asio::streambuf request_buf;
            boost::system::error_code error;

            // Читаем заголовки
            size_t len = boost::asio::read_until(socket, request_buf, "\r\n\r\n", error);
            if (error && error != boost::asio::error::eof) {
                std::cerr << "Ошибка чтения заголовков: " << error.message() << std::endl;
                return;
            }

            std::istream request_stream(&request_buf);
            std::string request_line;
            std::getline(request_stream, request_line);
            std::string method, path, version;

            std::istringstream req_line_stream(request_line);
            req_line_stream >> method >> path >> version;

            // Читаем остальные заголовки
            std::string header;
            size_t content_length = 0;
            while (std::getline(request_stream, header) && header != "\r") {
                if (header.find("Content-Length:") != std::string::npos) {
                    // Получаем длину тела
                    std::string cl = header.substr(header.find(":") + 1);
                    content_length = std::stoi(cl);
                }
            }

            std::string body;

            if (content_length > 0) {
                // читаем тело
                if (request_buf.size() < content_length) {
                    boost::asio::read(socket, request_buf, boost::asio::transfer_exactly(content_length - request_buf.size()), error);
                }
                std::istream body_stream(&request_buf);
                std::vector<char> buf(content_length);
                body_stream.read(buf.data(), content_length);
                body.assign(buf.begin(), buf.end());
            }

            // Распарсим POST-данные (например, application/x-www-form-urlencoded: слово=Значение)
            std::string word;
            auto pos = body.find("word=");
            if (pos != std::string::npos) {
                auto end_pos = body.find('&', pos);
                word = body.substr(pos + 5, end_pos - (pos + 5));
            }
            // std::cout << word;
            
            // Обработать строку и положить отдельные слова в вектор
            std::vector<std::string> words;
            splitByDelimiter(word, "%20", words);
            word = "";
            for (auto It = words.begin(); It != words.end(); It++) {
                word += (*It) + " ";
            };

            // отладка 
            /*
            for (auto It = words.begin(); It != words.end(); It++) {
                std::cout << (*It) << " ";
            };
            */

            // тут нужно возвращать число повоторений слов в БД
            // WordSearchEngineDatabase wordSearchEngineDatabase(word, count);
            // word - слово
            // count - число повторений
            int num1 = 123;
            // int num2 = 24;

            // Готовим HTML-ответ
            std::string html_response = replace_all_(html_response_template, "{word}", word);
            html_response = replace_all_(html_response, "{num1}", std::to_string(num1));
            // html_response = replace_all_(html_response, "{num2}", std::to_string(num2));

            // Формируем ответ
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Access-Control-Allow-Origin: *\r\n" // разрешение CORS для всех доменов
                "Content-Length: " + std::to_string(html_response.size()) + "\r\n"
                "\r\n" + html_response;

            boost::asio::write(socket, boost::asio::buffer(response), error);

        }
        catch (std::exception& e) {
            std::cerr << "Исключение: " << e.what() << std::endl;
        }
    }
    
}

