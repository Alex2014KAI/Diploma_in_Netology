#include "HTTPClient.h"

namespace SPIDER
{
    bool HTTPClientSinc::get(const std::string& target, std::string& out_body, std::string& out_error)
    {
        try
        {
            // ��������� ������
            auto const results = resolver_.resolve(host_, port_);

            // �����������
            stream_.connect(results);

            // ��������� HTTP GET-������
            http::request<http::empty_body> req{ http::verb::get, target, http_version_ };
            req.set(http::field::host, host_);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // �������� �������
            http::write(stream_, req);

            // ����� ��� ������
            beast::flat_buffer buffer;

            // ������ ��� �������� HTTP-������ � ������������ �����
            http::response<http::dynamic_body> res;

            // ������ ������
            http::read(stream_, buffer, res);

            // ���������� ���� ������ � ������
            out_body = beast::buffers_to_string(res.body().data());

            // ��������� ��������� ����������
            beast::error_code ec;
            stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
            // ���� ������ �� ������� � ���, ��� ���������� ��� �������
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
        // ��������� ����������� ���������� DNS
        resolver_.async_resolve(host_, "80",
            beast::bind_front_handler(&HTTPClient::on_resolve, shared_from_this()));
    }
    void HTTPClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // ������������� ����������
        stream_.async_connect(results,
            beast::bind_front_handler(&HTTPClient::on_connect, shared_from_this()));
    }

    void HTTPClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
            return fail(ec, "connect");

        // ��������� HTTP GET ������
        req_.version(http_version_);
        req_.method(http::verb::get);
        req_.target(target_);
        req_.set(http::field::host, host_);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // ���������� ������
        http::async_write(stream_, req_,
            beast::bind_front_handler(&HTTPClient::on_write, shared_from_this()));
    }

    void HTTPClient::on_write(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // �������� �����
        http::async_read(stream_, buffer_, res_,
            beast::bind_front_handler(&HTTPClient::on_read, shared_from_this()));
    }

    void HTTPClient::on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        // ������� ���� ������
        std::cout << res_.body() << std::endl;

        // ��������� ����������
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected)
            return fail(ec, "shutdown");
    }

    void HTTPClient::fail(beast::error_code ec, char const* what)
    {
        std::cerr << what << ": " << ec.message() << "\n";
    }


}