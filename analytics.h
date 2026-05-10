#pragma once
// analytics.h — 知识点薄弱分析
// 统计各知识点健康度，用柱状图展示，并基于标签共现提示关联薄弱区

#include "models.h"
#include "sm2_engine.h"
#include <map>
#include <algorithm>
#include <cmath>

// 单个标签的统计信息
struct TagStats {
    std::string tag;
    float health_score = 0.0f;  // 0.0（最薄弱）到 5.0（最强）
    int question_count = 0;     // 带此标签的题目数量
    float avg_mastery = 0.0f;   // 平均掌握程度
};

// 标签共现对（用于发现关联薄弱区）
struct TagCoOccurrence {
    std::string tag_a;
    std::string tag_b;
    int count = 0;                // 同时包含两个标签的题目数
    float combined_weakness = 0.0f; // 综合薄弱度（越高越薄弱）
};

// 计算所有标签的健康度统计
inline std::vector<TagStats> compute_tag_stats(const AppData& data) {
    SimpleDate today = get_today();
    // tag -> (加权分数总和, 题目数量, 掌握度总和)
    std::map<std::string, float> weighted_sum;
    std::map<std::string, int>   count;
    std::map<std::string, float> mastery_sum;

    for (const auto& q : data.questions) {
        auto tags = split_tags(q.tags);
        if (tags.empty()) tags.push_back("(无标签)");

        // 时效性权重：逾期越多权重越低（惩罚）
        int overdue = today - q.next_review; // 正数=逾期
        float recency = std::max(0.1f, 1.0f - overdue / 30.0f);
        float weighted_mastery = static_cast<float>(q.mastery) * recency;

        for (const auto& tag : tags) {
            weighted_sum[tag] += weighted_mastery;
            count[tag] += 1;
            mastery_sum[tag] += static_cast<float>(q.mastery);
        }
    }

    std::vector<TagStats> result;
    for (const auto& [tag, _] : weighted_sum) {
        TagStats s;
        s.tag = tag;
        s.question_count = count[tag];
        s.health_score = weighted_sum[tag] / count[tag];
        s.avg_mastery = mastery_sum[tag] / count[tag];
        result.push_back(s);
    }
    return result;
}

// 计算标签共现（关注掌握度 < 3 的薄弱题）
inline std::vector<TagCoOccurrence> compute_tag_co_occurrence(const AppData& data) {
    // 使用 map 做共现计数，key 为有序标签对
    std::map<std::pair<std::string, std::string>, int> co_count;
    std::map<std::pair<std::string, std::string>, float> co_weakness;

    for (const auto& q : data.questions) {
        // 只关注薄弱题（掌握度 < 3）
        if (q.mastery >= 3) continue;

        auto tags = split_tags(q.tags);
        if (tags.size() < 2) continue;

        // 所有标签两两配对
        for (size_t i = 0; i < tags.size(); ++i) {
            for (size_t j = i + 1; j < tags.size(); ++j) {
                auto key = std::make_pair(
                    std::min(tags[i], tags[j]),
                    std::max(tags[i], tags[j])
                );
                co_count[key] += 1;
                co_weakness[key] += (3.0f - q.mastery); // 越薄弱权重越高
            }
        }
    }

    std::vector<TagCoOccurrence> result;
    for (const auto& [pair, cnt] : co_count) {
        TagCoOccurrence c;
        c.tag_a = pair.first;
        c.tag_b = pair.second;
        c.count = cnt;
        c.combined_weakness = co_weakness[pair];
        result.push_back(c);
    }

    // 按综合薄弱度降序排序
    std::sort(result.begin(), result.end(),
              [](const TagCoOccurrence& a, const TagCoOccurrence& b) {
                  return a.combined_weakness > b.combined_weakness;
              });
    return result;
}

// 按健康度升序排序（最薄弱的排前面）
inline void sort_by_health(std::vector<TagStats>& stats) {
    std::sort(stats.begin(), stats.end(),
              [](const TagStats& a, const TagStats& b) {
                  return a.health_score < b.health_score;
              });
}
