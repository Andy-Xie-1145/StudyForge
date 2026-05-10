#pragma once
// persistence.h — .dat 文件读写
// 文件格式：
//   # 注释行
//   NEXT_ID:N
//   ---
//   !QUESTION
//   @id:1
//   @subject:数学
//   @stem:题干内容（\n 转义为 \\n）
//   @answer:答案内容
//   @error_reason:错误原因
//   @tags:标签1,标签2
//   @mastery:3
//   @sm2_rep:2
//   @sm2_ef:2.50
//   @sm2_interval:6
//   @last_review:2026-04-25
//   @next_review:2026-05-01
//   @last_quality:3
//   !END
//   ---

#include "models.h"
#include <fstream>
#include <iostream>

// 转义：将实际换行 \n 替换为字面 \\n
inline std::string escape_newlines(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\n') out += "\\n";
        else if (c == '\r') {} // 忽略 \r
        else out += c;
    }
    return out;
}

// 反转义：将字面 \\n 还原为实际换行
inline std::string unescape_newlines(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (i + 1 < s.size() && s[i] == '\\' && s[i + 1] == 'n') {
            out += '\n';
            ++i;
        } else {
            out += s[i];
        }
    }
    return out;
}

// 数据文件路径（相对于工作目录）
inline std::string get_data_filepath() {
    return "studyforge_data.dat";
}

// 从 .dat 文件加载数据
inline AppData load_data(const std::string& filepath) {
    AppData data;
    std::ifstream fin(filepath);
    if (!fin.is_open()) return data; // 文件不存在，返回空数据

    std::string line;
    while (std::getline(fin, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("NEXT_ID:", 0) == 0) {
            data.next_id = std::stoi(line.substr(8));
        } else if (line == "!QUESTION") {
            // 读取一个完整的题目块
            Question q;
            while (std::getline(fin, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') continue;
                if (line == "!END" || line == "---") break;

                if (line.rfind("@id:", 0) == 0)             q.id = std::stoi(line.substr(4));
                else if (line.rfind("@subject:", 0) == 0)   q.subject = line.substr(9);
                else if (line.rfind("@type:", 0) == 0) {
                    std::string ts = line.substr(6);
                    if (ts == "MC")        q.type = QTYPE_MC;
                    else if (ts == "FILL") q.type = QTYPE_FILL;
                    else                   q.type = QTYPE_ESSAY;
                }
                else if (line.rfind("@option_a:", 0) == 0)  q.option_a = unescape_newlines(line.substr(10));
                else if (line.rfind("@option_b:", 0) == 0)  q.option_b = unescape_newlines(line.substr(10));
                else if (line.rfind("@option_c:", 0) == 0)  q.option_c = unescape_newlines(line.substr(10));
                else if (line.rfind("@option_d:", 0) == 0)  q.option_d = unescape_newlines(line.substr(10));
                else if (line.rfind("@stem:", 0) == 0)      q.stem = unescape_newlines(line.substr(6));
                else if (line.rfind("@answer:", 0) == 0)    q.answer = unescape_newlines(line.substr(8));
                else if (line.rfind("@error_reason:", 0) == 0) q.error_reason = unescape_newlines(line.substr(14));
                else if (line.rfind("@tags:", 0) == 0)      q.tags = line.substr(6);
                else if (line.rfind("@mastery:", 0) == 0)   q.mastery = std::stoi(line.substr(9));
                else if (line.rfind("@sm2_rep:", 0) == 0)   q.sm2_repetition = std::stoi(line.substr(9));
                else if (line.rfind("@sm2_ef:", 0) == 0)    q.sm2_easiness_factor = std::stof(line.substr(8));
                else if (line.rfind("@sm2_interval:", 0) == 0) q.sm2_interval = std::stoi(line.substr(14));
                else if (line.rfind("@last_review:", 0) == 0) q.last_review = SimpleDate::from_string(line.substr(13));
                else if (line.rfind("@next_review:", 0) == 0) q.next_review = SimpleDate::from_string(line.substr(13));
                else if (line.rfind("@last_quality:", 0) == 0) q.last_quality = std::stoi(line.substr(14));
            }
            data.questions.push_back(q);
        } else if (line == "!EXAM_HISTORY") {
            ExamRecord rec;
            while (std::getline(fin, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') continue;
                if (line == "!END" || line == "---") break;
                if (line.rfind("@date:", 0) == 0)     rec.date = SimpleDate::from_string(line.substr(6));
                else if (line.rfind("@subject:", 0) == 0)  rec.subject = line.substr(9);
                else if (line.rfind("@total:", 0) == 0)    rec.total = std::stoi(line.substr(7));
                else if (line.rfind("@correct:", 0) == 0)  rec.correct = std::stoi(line.substr(9));
                else if (line.rfind("@graded:", 0) == 0)   rec.graded = std::stoi(line.substr(8));
            }
            data.exam_history.push_back(rec);
        }
    }
    return data;
}

// 保存数据到 .dat 文件
inline bool save_data(const AppData& data, const std::string& filepath) {
    std::ofstream fout(filepath, std::ios::trunc);
    if (!fout.is_open()) {
        std::cerr << "无法保存数据到: " << filepath << std::endl;
        return false;
    }

    fout << "# StudyForge 数据文件 — 学霸错题助手自动生成\n";
    fout << "NEXT_ID:" << data.next_id << "\n";

    for (const auto& q : data.questions) {
        fout << "---\n";
        fout << "!QUESTION\n";
        fout << "@id:" << q.id << "\n";
        fout << "@subject:" << q.subject << "\n";
        fout << "@type:" << (q.type == QTYPE_MC ? "MC" : q.type == QTYPE_FILL ? "FILL" : "ESSAY") << "\n";
        fout << "@option_a:" << escape_newlines(q.option_a) << "\n";
        fout << "@option_b:" << escape_newlines(q.option_b) << "\n";
        fout << "@option_c:" << escape_newlines(q.option_c) << "\n";
        fout << "@option_d:" << escape_newlines(q.option_d) << "\n";
        fout << "@stem:" << escape_newlines(q.stem) << "\n";
        fout << "@answer:" << escape_newlines(q.answer) << "\n";
        fout << "@error_reason:" << escape_newlines(q.error_reason) << "\n";
        fout << "@tags:" << q.tags << "\n";
        fout << "@mastery:" << q.mastery << "\n";
        fout << "@sm2_rep:" << q.sm2_repetition << "\n";
        fout << "@sm2_ef:" << std::fixed;
        fout.precision(2);
        fout << q.sm2_easiness_factor << "\n";
        fout << "@sm2_interval:" << q.sm2_interval << "\n";
        fout << "@last_review:" << q.last_review.to_string() << "\n";
        fout << "@next_review:" << q.next_review.to_string() << "\n";
        fout << "@last_quality:" << q.last_quality << "\n";
        fout << "!END\n";
    }
    fout << "---\n";

    // 考试历史
    for (const auto& rec : data.exam_history) {
        fout << "!EXAM_HISTORY\n";
        fout << "@date:" << rec.date.to_string() << "\n";
        fout << "@subject:" << rec.subject << "\n";
        fout << "@total:" << rec.total << "\n";
        fout << "@correct:" << rec.correct << "\n";
        fout << "@graded:" << rec.graded << "\n";
        fout << "!END\n";
        fout << "---\n";
    }

    return true;
}
