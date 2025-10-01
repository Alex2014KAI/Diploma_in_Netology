#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>


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

	/*
	class HTTPClient : public std::enable_shared_from_this<HTTPClient> {
	public:
		HTTPClient(net::io_context& ioc, const std::string& host, const std::string& target, int version = 11)
			: resolver_(ioc), stream_(ioc), host_(host), target_(target), http_version_(version) {};
		void run();

	private:
		tcp::resolver resolver_;
		beast::tcp_stream stream_;
		beast::flat_buffer buffer_; // Буфер для чтения
		http::request<http::empty_body> req_;
		http::response<http::string_body> res_;

		std::string host_;
		std::string target_;
		int http_version_;

		void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
		void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
		void on_write(beast::error_code ec, std::size_t bytes_transferred);
		void on_read(beast::error_code ec, std::size_t bytes_transferred);
		void fail(beast::error_code ec, char const* what);
	};
	*/
}

