#include "HTTPClient.h"

namespace SPIDER
{
    bool HTTPClientSinc::get(const std::string& target, std::string& out_body, std::string& out_error)
    {
        try
        {
            // Резолвинг адреса
            auto const results = resolver_.resolve(host_, port_);

            // Подключение
            stream_.connect(results);

            // Формируем HTTP GET-запрос
            http::request<http::empty_body> req{ http::verb::get, target, http_version_ };
            req.set(http::field::host, host_);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // Отправка запроса
            http::write(stream_, req);

            // Буфер для ответа
            beast::flat_buffer buffer;

            // Объект для хранения HTTP-ответа с динамическим телом
            http::response<http::dynamic_body> res;

            // Чтение ответа
            http::read(stream_, buffer, res);

            // Записываем тело ответа в строку
            out_body = beast::buffers_to_string(res.body().data());

            // Аккуратно закрываем соединение
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
            // Если ошибка не связана с тем, что соединение уже закрыто
            if (ec && ec != beast::errc::not_connected)
                throw beast::system_error{ ec };

            return true;
        }
        catch (std::exception const& e)
        {
            out_error = e.what();
            return false;
        }
    }

    void HTTPClient::run() {
        // Запускаем асинхронное разрешение DNS
        resolver_.async_resolve(host_, "80",
            beast::bind_front_handler(&HTTPClient::on_resolve, shared_from_this()));
    }
    void HTTPClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // Устанавливаем соединение
        stream_.async_connect(results,
            beast::bind_front_handler(&HTTPClient::on_connect, shared_from_this()));
    }

    void HTTPClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
            return fail(ec, "connect");

        // Формируем HTTP GET запрос
        req_.version(http_version_);
        req_.method(http::verb::get);
        req_.target(target_);
        req_.set(http::field::host, host_);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Отправляем запрос
        http::async_write(stream_, req_,
            beast::bind_front_handler(&HTTPClient::on_write, shared_from_this()));
    }

    void HTTPClient::on_write(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // Получаем ответ
        http::async_read(stream_, buffer_, res_,
            beast::bind_front_handler(&HTTPClient::on_read, shared_from_this()));
    }

    void HTTPClient::on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        // Выводим тело ответа
        std::cout << res_.body() << std::endl;

        // Закрываем соединение
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected)
            return fail(ec, "shutdown");
    }

    void HTTPClient::fail(beast::error_code ec, char const* what)
    {
        std::cerr << what << ": " << ec.message() << "\n";
    }


}