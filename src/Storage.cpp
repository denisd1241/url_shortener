#include "Storage.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Конструктор класса Storage, который принимает путь к файлу для хранения данных
Storage::Storage(const std::string& file_path) : file_path_(file_path) {
    // Проверка, существует ли файл, если нет — создает его
    std::ifstream file(file_path_);
    if (!file) {
        std::ofstream create_file(file_path_);
        if (create_file.is_open()) {
            std::cout << "Storage file not found. A new one has been created: " << file_path_ << std::endl;
        } else {
            std::cerr << "Failed to create storage file: " << file_path_ << std::endl;
            throw std::runtime_error("Failed to create storage file.");
        }
    }
    load();
}

// Загрузка данных из хранилища
void Storage::load() {
    std::ifstream file(file_path_);
    if (!file) {
        std::cerr << "Failed to open storage file for reading: " << file_path_ << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string short_url, original_url;
        // Извлечение короткого и оригинального URL из строки
        if (std::getline(iss, short_url, ',') && std::getline(iss, original_url)) {
            url_map_[short_url] = original_url; // Сохранение в map url 
        }
    }
}

// Сохранение в хранилище
void Storage::save() {
    std::ofstream file(file_path_, std::ofstream::trunc);
    if (!file) {
        std::cerr << "Failed to open storage file for writing: " << file_path_ << "\n";
        return;
    }

    // Сохраняем все записи из url_map_ в файл
    for (const auto& pair : url_map_) {
        file << pair.first << "," << pair.second << "\n"; // Формат: короткий URL, оригинальный URL
    }
    std::cout << "Storage successfully saved." << std::endl;
}

// добавление новой пары url
void Storage::add(const std::string& short_url, const std::string& original_url) {
    // Проверка на дублирующуюся ссылку
    for (const auto& pair : url_map_) {
        if (pair.second == original_url) {
            std::cout << "A short version already exists for this link: " << pair.first << std::endl;
            return; 
        }
    }
    url_map_[short_url] = original_url;
    std::cout << "New entry added: " << short_url << " -> " << original_url << std::endl;
}

// Получение оригинального URL по короткому
std::string Storage::get(const std::string& short_url) const {
    auto it = url_map_.find(short_url);
    return it != url_map_.end() ? it->second : ""; 
}

// Получение короткого URL по оригинальному
std::string Storage::get_short_url_by_original(const std::string& original_url) const {
    for (const auto& pair : url_map_) {
        if (pair.second == original_url) {
            return pair.first; 
        }
    }
    return ""; // Возвращаем пустую строку, если не найдено
}