#include "HTTPClient.h"

namespace SPIDER
{
    /*
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
    */

}