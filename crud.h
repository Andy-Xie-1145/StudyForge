#pragma once
// crud.h — 错题增删改查操作

#include "models.h"
#include <algorithm>
#include <set>

// 按 id 查找题目，返回指针（未找到返回 nullptr）
inline Question* find_question_by_id(AppData& data, int id) {
    for (auto& q : data.questions) {
        if (q.id == id) return &q;
    }
    return nullptr;
}

inline const Question* find_question_by_id(const AppData& data, int id) {
    for (const auto& q : data.questions) {
        if (q.id == id) return &q;
    }
    return nullptr;
}

// 添加新题目，返回分配的 id
inline int add_question(AppData& data,
                        const std::string& subject,
                        QuestionType type,
                        const std::string& stem,
                        const std::string& option_a,
                        const std::string& option_b,
                        const std::string& option_c,
                        const std::string& option_d,
                        const std::string& answer,
                        const std::string& error_reason,
                        const std::string& tags,
                        int mastery) {
    Question q;
    q.id = data.next_id++;
    q.subject = subject;
    q.type = type;
    q.stem = stem;
    q.option_a = option_a;
    q.option_b = option_b;
    q.option_c = option_c;
    q.option_d = option_d;
    q.answer = answer;
    q.error_reason = error_reason;
    q.tags = tags;
    q.mastery = std::clamp(mastery, 0, 5);
    // SM-2 初始值
    q.sm2_repetition = 0;
    q.sm2_easiness_factor = 2.5f;
    q.sm2_interval = 0;
    q.last_review = get_today();
    q.next_review = get_today(); // 立即可复习
    q.last_quality = 0;
    data.questions.push_back(q);
    return q.id;
}

// 编辑已有题目（按 id 定位）
inline bool edit_question(AppData& data, int id,
                          const std::string& subject,
                          QuestionType type,
                          const std::string& stem,
                          const std::string& option_a,
                          const std::string& option_b,
                          const std::string& option_c,
                          const std::string& option_d,
                          const std::string& answer,
                          const std::string& error_reason,
                          const std::string& tags,
                          int mastery) {
    Question* q = find_question_by_id(data, id);
    if (!q) return false;
    q->subject = subject;
    q->type = type;
    q->stem = stem;
    q->option_a = option_a;
    q->option_b = option_b;
    q->option_c = option_c;
    q->option_d = option_d;
    q->answer = answer;
    q->error_reason = error_reason;
    q->tags = tags;
    q->mastery = std::clamp(mastery, 0, 5);
    return true;
}

// 删除题目（按 id），永不复用 id
inline bool delete_question(AppData& data, int id) {
    auto it = std::remove_if(data.questions.begin(), data.questions.end(),
                             [id](const Question& q) { return q.id == id; });
    if (it == data.questions.end()) return false;
    data.questions.erase(it, data.questions.end());
    return true;
}

// 过滤/搜索题目，返回匹配的索引列表
inline std::vector<int> filter_questions(const AppData& data,
                                         const std::string& search_text,
                                         const std::string& subject_filter,
                                         const std::string& tag_filter) {
    std::vector<int> indices;
    for (int i = 0; i < (int)data.questions.size(); ++i) {
        const auto& q = data.questions[i];
        // 科目过滤
        if (!subject_filter.empty() && q.subject != subject_filter) continue;
        // 标签过滤（大小写无关子串匹配）
        if (!tag_filter.empty() && !ci_contains(q.tags, tag_filter)) continue;
        // 全文搜索（在题干、答案、错误原因、标签中搜索）
        if (!search_text.empty()) {
            bool found = ci_contains(q.stem, search_text) ||
                         ci_contains(q.answer, search_text) ||
                         ci_contains(q.error_reason, search_text) ||
                         ci_contains(q.tags, search_text) ||
                         ci_contains(q.subject, search_text);
            if (!found) continue;
        }
        indices.push_back(i);
    }
    return indices;
}

// 获取所有不重复的科目列表（排序）
inline std::vector<std::string> get_all_subjects(const AppData& data) {
    std::set<std::string> subjects;
    for (const auto& q : data.questions) {
        if (!q.subject.empty()) subjects.insert(q.subject);
    }
    return std::vector<std::string>(subjects.begin(), subjects.end());
}

// 获取所有不重复的标签列表（排序）
inline std::vector<std::string> get_all_tags(const AppData& data) {
    std::set<std::string> tag_set;
    for (const auto& q : data.questions) {
        auto tags = split_tags(q.tags);
        for (const auto& t : tags) tag_set.insert(t);
    }
    return std::vector<std::string>(tag_set.begin(), tag_set.end());
}
