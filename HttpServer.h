#pragma once


#include <boost/asio.hpp>
#include <iostream>
#include <string>


#include <vector>
#include <sstream>
#include <algorithm>

#include "WordSearchEngineDatabase.h"
#include "Spider.h"
#include "globals.h"

// #define WORK_KODE_DEBUG


namespace SPIDER
{
    inline void splitByDelimiter(const std::string& input, const std::string& delimiter, std::vector<std::string>& words) { // Для разделения слов пробелом или любым другим символом
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

            // Обработать строку и положить отдельные слова в вектор
            std::vector<std::string> words;
            splitByDelimiter(word, "%20", words);
            word = "";
            for (auto It = words.begin(); It != words.end(); It++) {
                word += (*It) + " ";
            };

            if (words.size() > MAX_NUMBER_OF_WORDS_TO_SEARCH_IN_THE_DATABASE) {
                words.resize(MAX_NUMBER_OF_WORDS_TO_SEARCH_IN_THE_DATABASE);
            }


            std::string documentURL{ "" };

            SPIDER::SpiderSetup spiderSetup("ini.txt");
            WordSearchEngineDatabase wordSearchEngineDatabase(spiderSetup.dataSetupBD_);
            std::vector<DocumentFrequency> documentFrequency;
            documentFrequency = wordSearchEngineDatabase.getDocumentsByWords(words);



            // Готовим HTML-ответ
            std::string html_response_template = "<html> <head><title>Ответ</title></head> <body> <h1>The word/combination of words entered: {word}</h1>"; // <p> Number of repetitions on sites: {num1}</p>
            std::string html_response;

            if (documentFrequency.empty()) {
                html_response_template += "<p> not found on any sites  </p>";
                html_response_template += " </body> </html>";
                html_response = html_response_template;
            }
            else {
                for (int i{ 0 }; i < documentFrequency.size(); i++) {
                    std::string countWord = "{countWord" + std::to_string(i) + "}";
                    std::string documentURL = "{documentURL" + std::to_string(i) + "}";
                    html_response_template += "<p> On the website: " + documentURL + ". The number of repetitions of words is: " + countWord + "</p>";
                }

                html_response_template += " </body> </html>";
                std::cout << html_response_template << std::endl;

                html_response = replace_all_(html_response_template, "{word}", word);
                for (int i{ 0 }; i < documentFrequency.size(); i++) {
                    std::string countWord = "{countWord" + std::to_string(i) + "}";
                    std::string documentURL = "{documentURL" + std::to_string(i) + "}";
                    html_response = replace_all_(html_response, documentURL, documentFrequency[i].url);
                    html_response = replace_all_(html_response, countWord, std::to_string(documentFrequency[i].total_frequency));
                };
            }
            

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

