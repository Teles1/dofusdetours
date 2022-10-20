#pragma once
#include <asio.hpp>
#include <iostream>
using asio::ip::tcp;
#define MAX_LENGTH 8192
struct tcp_client : public std::enable_shared_from_this<tcp_client>
{
    tcp_client(asio::io_context& p_context) : buffer_{}, io_context_(p_context), socket_(p_context), connected_(false)
    {
    }

    asio::error_code start()
    {
        tcp::resolver resolver(io_context_);
        asio::error_code ec;
        endpoint_ = asio::connect(socket_, resolver.resolve("127.0.0.1", "5555"), ec);
        if(!ec)
        {
            connected_ = true;
            do_read();
        }
        return ec;
    }

    size_t send(char* data, const int length)
    {
        asio::error_code ec;
        const auto result = asio::write(socket_, asio::buffer(data, length), ec);
        if (ec)
            std::cout << std::format("write({})=> {}", ec.value(), ec.message()) << std::endl;
        return result;
    }

    std::string get_address() const
    {
        return endpoint_.address().to_string();
    }
    bool is_connected() const
    {
        return connected_;
    }
private:
    void do_read()
    {
        socket_.async_read_some(asio::buffer(buffer_, MAX_LENGTH), [this](asio::error_code ec, std::size_t /*length*/)
            {
                if (ec)
                {
                    connected_ = false;
                    std::cout << std::format("do_read({})=> {}", ec.value(), ec.message()) << std::endl;
                    return;
                }
                //if no socket errors we can continue reading.
                do_read();
            });
    }

private:
    char buffer_[MAX_LENGTH];
    asio::io_context& io_context_;
    tcp::socket socket_;
    asio::ip::basic_endpoint<tcp> endpoint_;
    std::mutex mutex_;
    bool connected_;
};