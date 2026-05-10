#pragma once
// models.h — 数据结构与日期工具函数
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <sstream>

// ============================================================
// 辅助函数
// ============================================================

// 去除字符串两端空白
inline std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// 按逗号分割标签字符串，返回去空白的标签列表
inline std::vector<std::string> split_tags(const std::string& tags_str) {
    std::vector<std::string> result;
    std::istringstream iss(tags_str);
    std::string token;
    while (std::getline(iss, token, ',')) {
        std::string t = trim(token);
        if (!t.empty()) result.push_back(t);
    }
    return result;
}

// 大小写无关的子串搜索
inline bool ci_contains(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    auto it = std::search(haystack.begin(), haystack.end(),
                          needle.begin(), needle.end(),
                          [](char a, char b) {
                              return std::tolower(static_cast<unsigned char>(a)) ==
                                     std::tolower(static_cast<unsigned char>(b));
                          });
    return it != haystack.end();
}

// ============================================================
// SimpleDate — 轻量级日期结构体（C++17 兼容）
// ============================================================

struct SimpleDate {
    int year  = 0;
    int month = 1;  // 1-12
    int day   = 1;  // 1-31

    SimpleDate() = default;
    SimpleDate(int y, int m, int d) : year(y), month(m), day(d) {}

    // 转换为绝对日计数（类似儒略日，便于日期运算）
    int to_absolute() const {
        // 将 3月视为每年的第一个月，方便闰年处理
        int a = (14 - month) / 10;
        int y = year + 4800 - a;
        int m = month + 12 * a - 3;
        return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    }

    // 从绝对日计数构造日期
    static SimpleDate from_absolute(int jd) {
        int a = jd + 32044;
        int b = (4 * a + 3) / 146097;
        int c = a - (146097 * b) / 4;
        int d = (4 * c + 3) / 1461;
        int e = c - (1461 * d) / 4;
        int m = (5 * e + 2) / 153;
        SimpleDate date;
        date.day   = e - (153 * m + 2) / 5 + 1;
        date.month = m + 3 - 12 * (m / 10);
        date.year  = 100 * b + d - 4800 + m / 10;
        return date;
    }

    // 格式化为 YYYY-MM-DD
    std::string to_string() const {
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
        return std::string(buf);
    }

    // 从 YYYY-MM-DD 字符串解析
    static SimpleDate from_string(const std::string& s) {
        SimpleDate d;
        if (s.size() >= 10 && s[4] == '-' && s[7] == '-') {
            d.year  = std::stoi(s.substr(0, 4));
            d.month = std::stoi(s.substr(5, 2));
            d.day   = std::stoi(s.substr(8, 2));
        }
        return d;
    }

    // 日期 + N 天
    SimpleDate operator+(int days) const {
        return from_absolute(to_absolute() + days);
    }

    // 日期差（天数）
    int operator-(const SimpleDate& other) const {
        return to_absolute() - other.to_absolute();
    }

    bool operator<(const SimpleDate& o)  const { return to_absolute() <  o.to_absolute(); }
    bool operator<=(const SimpleDate& o) const { return to_absolute() <= o.to_absolute(); }
    bool operator>(const SimpleDate& o)  const { return to_absolute() >  o.to_absolute(); }
    bool operator==(const SimpleDate& o) const { return year == o.year && month == o.month && day == o.day; }
    bool operator!=(const SimpleDate& o) const { return !(*this == o); }

    bool is_valid() const {
        if (year < 1 || month < 1 || month > 12 || day < 1) return false;
        // 简单的月份天数检查
        static const int mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        int maxd = mdays[month - 1];
        if (month == 2) {
            bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (leap) maxd = 29;
        }
        return day <= maxd;
    }
};

// 获取今天的日期
inline SimpleDate get_today() {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    return SimpleDate(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

// ============================================================
// Question — 错题数据结构
// ============================================================

enum QuestionType { QTYPE_MC = 0, QTYPE_FILL = 1, QTYPE_ESSAY = 2 };

inline const char* question_type_label(QuestionType t) {
    if (t == QTYPE_MC)   return "\xe9\x80\x89\xe6\x8b\xa9\xe9\xa2\x98"; // 选择题
    if (t == QTYPE_FILL) return "\xe5\xa1\xab\xe7\xa9\xba\xe9\xa2\x98"; // 填空题
    return "\xe8\xa7\xa3\xe7\xad\x94\xe9\xa2\x98"; // 解答题
}

struct Question {
    // --- 核心字段 ---
    int id = 0;                     // 唯一标识符，自动递增
    std::string subject;            // 科目（如 "数学"、"英语"）
    QuestionType type = QTYPE_ESSAY; // 题型（默认解答题，兼容旧数据）
    std::string stem;               // 题干（多行）
    std::string option_a;           // 选择题选项A
    std::string option_b;           // 选择题选项B
    std::string option_c;           // 选择题选项C
    std::string option_d;           // 选择题选项D
    std::string answer;             // 答案（选择题存 A/B/C/D，其余为文本）
    std::string error_reason;       // 错误原因（多行）
    std::string tags;               // 知识点标签，逗号分隔（如 "代数,二次方程"）
    int mastery = 0;                // 掌握程度 0-5

    // --- SM-2 调度字段 ---
    int   sm2_repetition       = 0; // 连续成功重复次数
    float sm2_easiness_factor  = 2.5f; // 难度因子，初始 2.5
    int   sm2_interval         = 0;    // 当前间隔天数
    SimpleDate last_review;           // 上次复习日期
    SimpleDate next_review;           // 下次复习日期
    int   last_quality         = 0;    // 上次复习质量评分
};

// ============================================================
// 自动批改结果
// ============================================================

enum AutoResult { AUTO_CORRECT, AUTO_INCORRECT, AUTO_UNCERTAIN };

// ============================================================
// ExamRecord — 考试历史记录
// ============================================================

struct ExamRecord {
    SimpleDate date;
    std::string subject;
    int total = 0;
    int correct = 0;
    int graded = 0;
};

// ============================================================
// AppData — 应用全局数据
// ============================================================

struct AppData {
    std::vector<Question> questions;
    std::vector<ExamRecord> exam_history;
    int next_id = 1;  // 下一个可用的 ID，永不递减
};
