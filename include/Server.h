#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "Storage.h"
#include <string>

using boost::asio::ip::tcp;

// Класс Server отвечает за управление сервером, который обрабатывает входящие HTTP-запросы.
class Server {
public:
    Server(const std::string& address, const std::string& port, const std::string& storage_file);
    void run();

private:
    void handle_request(tcp::socket socket);
    boost::asio::io_service io_service_;
    tcp::acceptor acceptor_;
    Storage storage_;
    std::string address_; 
    std::string port_; 
};

#endif // SERVER_H
