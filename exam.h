#pragma once
// exam.h — 交互式考试引擎
// 选择科目和题数，按复习紧急度排序抽题，逐题作答后自动/手动评分

#include "models.h"
#include "sm2_engine.h"
#include <algorithm>
#include <cctype>

// ============================================================
// 考试数据结构
// ============================================================

struct ExamAnswer {
    int question_index;           // 在 AppData.questions 中的索引
    std::string user_response;    // 用户的作答内容
    bool auto_graded = false;     // 是否已自动批改（选择题）
    AutoResult auto_result = AUTO_UNCERTAIN; // 自动批改/匹配结果
    bool is_correct = false;      // 自动批改结果（仅选择题有效）
    int  self_grade = -1;         // 自评: -1=未评, 0=错误, 1=正确
};

struct ExamSession {
    enum Phase { CONFIG, QUIZZING, GRADING };
    Phase phase = CONFIG;
    std::string subject;
    int requested_count = 0;
    std::vector<int> selected_indices;   // 选中题目在 AppData.questions 中的索引
    std::vector<ExamAnswer> answers;     // 逐题作答记录
    int current = 0;                     // 当前题号（在 selected_indices 中的下标）
};

// ============================================================
// 文本标准化
// ============================================================

// 全角字母数字转半角，压缩连续空格，trim
inline std::string normalize_text(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    bool last_space = true; // 开头的空格也跳过
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        // 全角字母/数字: ０-９ (0xFF10-0xFF19), Ａ-Ｚ (0xFF21-0xFF3A), ａ-ｚ (0xFF41-0xFF5A)
        if (c == 0xEF && i + 2 < s.size()) {
            unsigned char c2 = static_cast<unsigned char>(s[i + 1]);
            unsigned char c3 = static_cast<unsigned char>(s[i + 2]);
            char half = 0;
            if (c2 == 0xBC && c3 >= 0x90 && c3 <= 0x99) half = '0' + (c3 - 0x90);      // ０-９
            else if (c2 == 0xBC && c3 >= 0xA1 && c3 <= 0xBA) half = 'A' + (c3 - 0xA1); // Ａ-Ｚ
            else if (c2 == 0xBD && c3 >= 0x81 && c3 <= 0x9A) half = 'a' + (c3 - 0x81); // ａ-ｚ
            if (half) {
                c = static_cast<unsigned char>(half);
                i += 2; // 跳过全角后两字节
            } else {
                out += s[i]; continue;
            }
        }
        if (std::isspace(c)) {
            if (!last_space) { out += ' '; last_space = true; }
        } else {
            out += static_cast<char>(std::tolower(c));
            last_space = false;
        }
    }
    return trim(out);
}

// ============================================================
// 考试逻辑
// ============================================================

// 开始考试：按紧急度选题
inline void exam_start(ExamSession& session, const AppData& data,
                       const std::string& subject, int count) {
    session = ExamSession();
    session.subject = subject;
    session.requested_count = count;

    std::vector<std::pair<int, int>> candidates;
    for (int i = 0; i < (int)data.questions.size(); ++i) {
        const auto& q = data.questions[i];
        if (!subject.empty() && q.subject != subject) continue;
        int overdue = sm2_overdue_days(q);
        candidates.emplace_back(i, overdue);
    }

    std::sort(candidates.begin(), candidates.end(),
              [&](const auto& a, const auto& b) {
                  if (a.second != b.second) return a.second > b.second;
                  return data.questions[a.first].mastery < data.questions[b.first].mastery;
              });

    int take = std::min(count, (int)candidates.size());
    for (int i = 0; i < take; ++i) {
        session.selected_indices.push_back(candidates[i].first);
    }
    session.answers.resize(take);
    for (int i = 0; i < take; ++i) {
        session.answers[i].question_index = session.selected_indices[i];
    }

    session.phase = ExamSession::QUIZZING;
    session.current = 0;
}

// 自动批改：选择题精确匹配，填空题标准化后匹配（不匹配为 UNCERTAIN），解答题 UNCERTAIN
inline AutoResult exam_auto_grade(const Question& q, const std::string& user_response) {
    if (q.type == QTYPE_MC) {
        if (user_response.empty()) return AUTO_INCORRECT;
        char u = (char)std::toupper((unsigned char)user_response[0]);
        char c = (char)std::toupper((unsigned char)q.answer[0]);
        return (u == c) ? AUTO_CORRECT : AUTO_INCORRECT;
    }
    if (q.type == QTYPE_FILL) {
        std::string a = normalize_text(user_response);
        std::string b = normalize_text(q.answer);
        return (a == b) ? AUTO_CORRECT : AUTO_UNCERTAIN;
    }
    return AUTO_UNCERTAIN; // 解答题
}

// 进入评分阶段时调用：选择题自动批改+自评，填空题只记参考
inline void exam_compute_summary(ExamSession& session, const AppData& data) {
    for (int i = 0; i < (int)session.answers.size(); ++i) {
        const auto& q = data.questions[session.answers[i].question_index];
        auto& ans = session.answers[i];
        ans.auto_result = exam_auto_grade(q, ans.user_response);
        if (q.type == QTYPE_MC) {
            ans.auto_graded = true;
            ans.is_correct = (ans.auto_result == AUTO_CORRECT);
            ans.self_grade = ans.is_correct ? 1 : 0;
        }
        // 填空题/解答题：需自评
    }
}
