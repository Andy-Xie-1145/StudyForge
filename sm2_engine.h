#pragma once
// sm2_engine.h — 基于 SM-2 算法的间隔重复复习引擎
// 算法参考：Piotr Wozniak, SuperMemo SM-2 (1987)

#include "models.h"

// 质量评分的中文标签
inline const char* quality_label(int q) {
    static const char* labels[] = {
        "\xe5\xae\x8c\xe5\x85\xa8\xe9\x81\x97\xe5\xbf\x98",   // 0: 完全遗忘
        "\xe6\x9c\x89\xe5\x8d\xb0\xe8\xb1\xa1",                 // 1: 有印象
        "\xe4\xbc\xbc\xe6\x9b\xbe\xe7\x9b\xb8\xe8\xaf\x86",   // 2: 似曾相识
        "\xe5\x8b\x89\xe5\xbc\xba\xe7\xad\x94\xe5\xaf\xb9",   // 3: 勉强答对
        "\xe6\xad\xa3\xe7\xa1\xae\xe4\xbd\x86\xe7\x8a\xb9\xe8\xb1\xab", // 4: 正确但犹豫
        "\xe5\xae\x8c\xe7\xbe\x8e"                               // 5: 完美
    };
    if (q >= 0 && q <= 5) return labels[q];
    return "\xe6\x9c\xaa\xe7\x9f\xa5"; // 未知
}

// 掌握程度的中文标签
inline const char* mastery_label(int m) {
    static const char* labels[] = {
        "\xe6\x9c\xaa\xe6\x8e\x8c\xe6\x8f\xa1",   // 0: 未掌握
        "\xe6\x9e\x81\xe5\xb7\xae",                 // 1: 极差
        "\xe8\xbe\x83\xe5\xb7\xae",                 // 2: 较差
        "\xe4\xb8\x80\xe8\x88\xac",                 // 3: 一般
        "\xe8\x89\xaf\xe5\xa5\xbd",                 // 4: 良好
        "\xe4\xbc\x98\xe7\xa7\x80"                   // 5: 优秀
    };
    if (m >= 0 && m <= 5) return labels[m];
    return "\xe6\x9c\xaa\xe7\x9f\xa5"; // 未知
}

// 应用 SM-2 算法，根据质量评分更新题目的调度字段
inline void sm2_update(Question& q, int quality) {
    quality = std::clamp(quality, 0, 5);
    q.last_quality = quality;
    q.mastery = quality; // 同步掌握程度
    SimpleDate today = get_today();
    q.last_review = today;

    if (quality >= 3) {
        // 成功回忆
        q.sm2_repetition += 1;
        if (q.sm2_repetition == 1) {
            q.sm2_interval = 1;   // 第 1 次成功：1 天后复习
        } else if (q.sm2_repetition == 2) {
            q.sm2_interval = 6;   // 第 2 次成功：6 天后复习
        } else {
            // 后续：间隔 = 上次间隔 × EF（四舍五入）
            q.sm2_interval = static_cast<int>(q.sm2_interval * q.sm2_easiness_factor + 0.5f);
            if (q.sm2_interval < 1) q.sm2_interval = 1;
        }
    } else {
        // 回忆失败：重置
        q.sm2_repetition = 0;
        q.sm2_interval = 1;
    }

    // 更新难度因子
    float ef = q.sm2_easiness_factor
             + (0.1f - (5 - quality) * (0.08f + (5 - quality) * 0.02f));
    if (ef < 1.3f) ef = 1.3f;
    q.sm2_easiness_factor = ef;

    // 计算下次复习日期
    q.next_review = today + q.sm2_interval;
}

// 判断题目是否到期需要复习
inline bool sm2_is_due(const Question& q) {
    return q.next_review <= get_today();
}

// 获取所有到期题目的索引
inline std::vector<int> sm2_get_due_indices(const AppData& data) {
    SimpleDate today = get_today();
    std::vector<int> indices;
    for (int i = 0; i < (int)data.questions.size(); ++i) {
        if (data.questions[i].next_review <= today) {
            indices.push_back(i);
        }
    }
    return indices;
}

// 计算逾期天数（正数=逾期，负数=尚未到期）
inline int sm2_overdue_days(const Question& q) {
    return get_today() - q.next_review;
}
