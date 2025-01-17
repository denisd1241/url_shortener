#include "Server.h"  
#include <iostream>  

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <address> <port> <storage_file>\n"; // Вывод сообщения о неправильном использовании
        return 1;  
    }
    try {
        // Создание экземпляра сервера с указанными параметрами: адрес, порт и файл хранения
        Server server(argv[1], argv[2], argv[3]);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n"; 
    }

    return 0;
}