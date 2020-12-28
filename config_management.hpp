#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
class config_management {
public:
    using config_item_type = std::unordered_map<std::string, std::string>; // key -- iterm type value -- iterm value
    using config_section_type = std::unordered_map<std::string, config_item_type>;    // key -- section key
public:
    config_management() = default;
    virtual ~config_management() = default;
public:
    bool parse(const char *config_path) {
        std::ifstream ifs(config_path, std::ios::in);
        if (!ifs || !ifs.is_open()) {
            std::cerr << "config file path:" << config_path << " open failed." << std::endl;
            return false;
        }
        std::string line;
        std::string section;
        std::pair<std::string, std::string>key_value;
        config_item_type *pItem = nullptr;
        while (getline(ifs, line, '\n')) {
            process_config_line(line);
            if (line.empty()) {
                continue;
            }
            if (extract_section(line, section)) {
                pItem = &config_section_map_[section];
            }
            else if (pItem && extract_iterm(line, key_value)) {
                pItem->insert(key_value);
            }
        }
        ifs.close();
        return true;
    }
    void dump_config() {
        for (auto &section : config_section_map_) {
            std::cout << "section:" << section.first << std::endl;
            for (auto &item : section.second) {
                std::cout << "key:" << item.first << std::endl;
                std::cout << "value:" << item.second << std::endl;
            }
        }
    }
    bool get_value(const char *section, const char *key, std::string &value) {
        auto it = config_section_map_.find(section);
        if (std::end(config_section_map_) == it) {
            std::cerr << "section:" << section << " not found." << std::endl;
            return false;
        }
        auto iterm_map = it->second;
        auto iterm_it = iterm_map.find(key);
        if (std::end(iterm_map) == iterm_it) {
            std::cerr << "key:" << key << " not found." << std::endl;
            return false;
        }
        value = iterm_it->second;
        return true;
    }
private:
    inline void process_config_line(std::string &line) {
        if (line.empty() || (std::string::npos == line.find_first_not_of(" \r\n\t")) || (COMMENT == line[0])) {     // there is no normal word
            line.clear();
            return;
        }
    }
    bool extract_iterm(const std::string &line, std::pair<std::string, std::string>&key_value) {
        auto pos = line.find(SEPARATOR);
        if (std::string::npos == pos) {
            return false;
        }
        key_value.first = line.substr(0, pos);
        key_value.second = line.substr(pos + 1);
        return true;
    }
    bool extract_section(const std::string &line, std::string &section) {
        auto pos = line.find(SEPARATOR);
        if (pos != std::string::npos) {
            return false;
        }
        auto start_pos = line.find(SECTION_START);
        if (std::string::npos == start_pos) {
            return false;
        }
        auto end_pos = line.find(SECTION_END);
        if (std::string::npos == end_pos) {
            return false;
        }
        section = line.substr(start_pos + 1, end_pos - start_pos - 1);
        return true;
    }
private:
    config_section_type config_section_map_;
private:
    const char COMMENT = '#';
    const char SEPARATOR = '=';
    const char SECTION_START = '[';
    const char SECTION_END = ']';
};