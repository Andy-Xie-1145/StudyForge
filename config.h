#pragma once
// config.h — 应用配置读写（studyforge_config.ini）
// 格式：每行 key=value，# 开头为注释

#include <string>
#include <fstream>
#include <sstream>

struct StudyConfig {
    std::string api_key;
    std::string model_url = "https://api.openai.com/v1/chat/completions";
    std::string model_name = "gpt-4o-mini";
};

inline StudyConfig load_config(const std::string& path = "studyforge_config.ini") {
    StudyConfig cfg;
    std::ifstream fin(path);
    if (!fin.is_open()) return cfg;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        if (key == "api_key")     cfg.api_key = val;
        else if (key == "model_url") cfg.model_url = val;
        else if (key == "model_name") cfg.model_name = val;
    }
    return cfg;
}

inline bool save_config(const StudyConfig& cfg, const std::string& path = "studyforge_config.ini") {
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return false;
    fout << "# StudyForge 配置文件\n";
    fout << "api_key=" << cfg.api_key << "\n";
    fout << "model_url=" << cfg.model_url << "\n";
    fout << "model_name=" << cfg.model_name << "\n";
    return true;
}
