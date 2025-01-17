#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <unordered_map>

// Класс Storage отвечает за хранение соответствий между короткими и оригинальными URL.
class Storage {
public:
    Storage(const std::string& file_path);
    std::string get_short_url_by_original(const std::string& original_url) const;  
    void add(const std::string& short_url, const std::string& original_url);
    std::string get(const std::string& short_url) const;
    void load();
    void save();

private:
    std::unordered_map<std::string, std::string> url_map_;
    std::string file_path_;
};

#endif // STORAGE_H
