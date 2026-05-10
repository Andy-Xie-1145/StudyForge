#pragma once
// ai.h — AI 辅助评判骨架（当前为占位实现）
// 未来接入 OpenAI 兼容 API 后替换函数体即可

#include "config.h"
#include <string>

struct AIGradeResult {
    float score = -1.0f;         // 0.0~5.0，-1 表示未评分
    std::string comment;         // AI 评语
};

inline AIGradeResult ai_judge_essay(const StudyConfig& /*cfg*/,
                                     const std::string& /*stem*/,
                                     const std::string& /*reference_answer*/,
                                     const std::string& /*user_answer*/) {
    // 骨架：未接入 API，返回未配置提示
    AIGradeResult r;
    r.score = -1.0f;
    r.comment = "AI \xe5\x8a\x9f\xe8\x83\xbd\xe6\x9c\xaa\xe9\x85\x8d\xe7\xbd\xae"; // AI功能未配置
    return r;
}

inline std::string ai_generate_explanation(const StudyConfig& /*cfg*/,
                                            const std::string& /*stem*/,
                                            const std::string& /*answer*/) {
    return "AI \xe5\x8a\x9f\xe8\x83\xbd\xe6\x9c\xaa\xe9\x85\x8d\xe7\xbd\xae"; // AI功能未配置
}
