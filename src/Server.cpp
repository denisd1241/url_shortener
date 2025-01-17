#include "Server.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <json/json.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Storage.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

// Генерация короткого URL из случайных символов
std::string generate_short_url() {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);

    std::string short_url;
    for (int i = 0; i < 6; ++i) {  // Длина короткой ссылки
        short_url += chars[dis(gen)];
    }
    return short_url;
}
// Конструктор класса Server
Server::Server(const std::string& address, const std::string& port, const std::string& storage_file)
    : io_service_(), // Инициализация io_service
      acceptor_(io_service_, tcp::endpoint(boost::asio::ip::make_address(address), std::stoi(port))), // Настройка для принятия соединений на заданном адресе и порту
      storage_(storage_file), // Инициализиция объекта storage с файлом хранения
      address_(address),
      port_(port) {
    storage_.load(); // Загрузка данных из файла хранения
}

//Метод для запуска сервера
void Server::run() {
    acceptor_.listen();
    std::cout << "Waiting for incoming requests..."  << std::endl;
    while (true) {
        tcp::socket socket(io_service_);
        acceptor_.accept(socket);
        handle_request(std::move(socket));
    }
}

//Обработка входящих HTTP-запросов,
void Server::handle_request(tcp::socket socket) {
    try {
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;

        // Чтение запроса из сокета
        http::read(socket, buffer, req);

        // Обработка Post-запроса 
        if (req.method() == http::verb::post && req.target() == "/shorten") {
            Json::Value root;
            std::istringstream(req.body()) >> root;  // парсим тело запроса
            if (!root.isMember("url") || root["url"].asString().empty()) {
                // Формируем ответ с ошибкой 400 если не найдено поле url в теле запроса
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::content_type, "application/json");
                Json::Value error_response;
                error_response["error"] = "Field 'url' is required";
                std::ostringstream error_body;
                error_body << error_response<< "\n";
                res.body() = error_body.str();
                res.prepare_payload();
                http::write(socket, res);
                return;  
            }
            std::string original_url = root["url"].asString();
            
            // Проверка, есть ли уже такой URL в базе данных
            std::string short_url = storage_.get_short_url_by_original(original_url);
            if (short_url.empty()) {
                // Генерация нового короткого URL
                short_url = generate_short_url();
                storage_.add(short_url, original_url);
                storage_.save();
                std::cout << "Created short URL: " << short_url << " for " << original_url << std::endl;
            } else {
                std::cout << "Using existing short URL: " << short_url << " for " << original_url << std::endl;
            }

            // Формируем полный короткий URL
            std::string full_short_url = "http://" + address_ + ":" + port_ + "/" + short_url;

            // Формируем JSON-ответ
            Json::Value response;
            response["short_url"] = full_short_url;  
            std::ostringstream response_body;
            response_body << response << "\n"; 

            // Формируем ответ HTTP
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::content_type, "application/json");
            res.body() = response_body.str();
            res.prepare_payload();
            http::write(socket, res);
        } 
        // Обработка GET-запроса для перенаправления
        else if (req.method() == http::verb::get) {
            std::string short_url = req.target().substr(1);  // Убираем "/"
            std::string original_url = storage_.get(short_url);

            if (!original_url.empty()) {
                // Перенаправление
                http::response<http::empty_body> res{http::status::moved_permanently, req.version()};
                res.set(http::field::location, original_url);
                http::write(socket, res);
            } else {
                // Ошибка 404 если короткий URL не найден
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::content_type, "application/json");
                Json::Value error_response;
                error_response["error"] = "URL not found";
                std::ostringstream error_body;
                error_body << error_response <<"\n";
                res.body() = error_body.str();
                res.prepare_payload();
                http::write(socket, res);
            }   
        }
    } catch (const boost::beast::system_error& e) {
        // Обработка ошибок Boost.Beast
        std::cerr << "Boost.Beast Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        // Обработка других ошибок
        std::cerr << "Error handling request: " << e.what() << std::endl;
    }
}
