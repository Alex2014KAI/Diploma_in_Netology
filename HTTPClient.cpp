#include "HTTPClient.h"

namespace SPIDER
{
    /*
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
    */

}