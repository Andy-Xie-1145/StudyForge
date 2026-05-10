// main.cpp — 学霸错题助手 (StudyForge) 应用主体
// Dear ImGui + GLFW + OpenGL3 桌面应用

#include "models.h"
#include "crud.h"
#include "sm2_engine.h"
#include "analytics.h"
#include "exam.h"
#include "persistence.h"
#include "config.h"
#include "ai.h"

// glad 必须在 GLFW 之前包含，且 GLFW 不能自动包含系统 GL 头文件
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

// ============================================================
// UI 状态
// ============================================================

// 复习会话状态
struct ReviewSession {
    std::vector<int> due_indices;   // 到期题目索引
    int current = 0;                // 当前显示的题目在 due_indices 中的下标
    bool showing_answer = false;    // 是否正在显示答案/批改
    int selected_quality = -1;      // 用户选择的质量评分
    bool active = false;            // 会话是否进行中
    int total_reviewed = 0;         // 本轮已复习题数
    bool completed = false;         // 本轮是否已完成
    char user_response[4096] = "";  // 用户作答（填空/解答题）
    int mc_selection = -1;          // 选择题选项：0=A,1=B,2=C,3=D
};

// ============================================================
// 前向声明：各标签页渲染函数
// ============================================================

static void render_question_management(AppData& data);
static void render_today_review(AppData& data);
static void render_weakness_analysis(const AppData& data);
static void render_exam_generator(AppData& data);

// ============================================================
// 工具函数：截取题干预览（最多前 N 个字符，换行替换为空格）
// ============================================================

static std::string stem_preview(const std::string& stem, size_t max_len = 40) {
    std::string s;
    for (char c : stem) {
        if (c == '\n' || c == '\r') { s += ' '; }
        else s += c;
        if (s.size() >= max_len) { s += "..."; break; }
    }
    return s;
}

// ============================================================
// 标签页 1：错题管理
// ============================================================

static void render_question_management(AppData& data) {
    // --- 搜索与过滤 ---
    static char search_buf[256] = "";
    static int  subject_idx = 0;   // 0 = 全部
    static char tag_filter[128] = "";

    ImGui::PushItemWidth(200);
    ImGui::InputText("搜索", search_buf, sizeof(search_buf));
    ImGui::SameLine();

    // 科目下拉框（动态构建）
    auto subjects = get_all_subjects(data);
    // 构建显示字符串
    static std::string combo_str;
    combo_str = "全部";
    for (const auto& s : subjects) { combo_str += '\0'; combo_str += s; }
    combo_str += '\0';
    ImGui::PushItemWidth(120);
    ImGui::Combo("科目", &subject_idx, combo_str.c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(150);
    ImGui::InputText("标签", tag_filter, sizeof(tag_filter));
    ImGui::PopItemWidth();
    ImGui::PopItemWidth();

    std::string subject_filter = (subject_idx == 0) ? "" : subjects[subject_idx - 1];
    auto filtered = filter_questions(data, search_buf, subject_filter, tag_filter);

    ImGui::Separator();

    // --- 题目表格 ---
    if (filtered.empty()) {
        ImGui::TextDisabled("还没有错题，点击下方按钮添加新题。");
    } else {
        // 使用 ImGui 表格
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
        ImVec2 table_size(0.0f, ImGui::GetContentRegionAvail().y - 40.0f);
        if (ImGui::BeginTable("questions_table", 7, flags, table_size)) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("ID",     ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableSetupColumn("科目",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("类型",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("题干",   ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("掌握",   ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("下次复习", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("操作",   ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();

            for (int idx : filtered) {
                const auto& q = data.questions[idx];
                ImGui::TableNextRow();
                ImGui::PushID(q.id);

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", q.id);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(q.subject.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(question_type_label(q.type));

                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(stem_preview(q.stem).c_str());
                if (ImGui::IsItemHovered() && q.stem.size() > 40) {
                    ImGui::SetTooltip("%s", q.stem.c_str());
                }

                ImGui::TableSetColumnIndex(4);
                // 掌握度进度条
                float frac = static_cast<float>(q.mastery) / 5.0f;
                char overlay[8];
                snprintf(overlay, sizeof(overlay), "%d/5", q.mastery);
                ImGui::ProgressBar(frac, ImVec2(-1, 0), overlay);

                ImGui::TableSetColumnIndex(5);
                SimpleDate today = get_today();
                bool overdue = q.next_review <= today;
                if (overdue) {
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s",
                                       q.next_review.to_string().c_str());
                } else {
                    ImGui::TextUnformatted(q.next_review.to_string().c_str());
                }

                ImGui::TableSetColumnIndex(6);
                if (ImGui::SmallButton("编辑")) {
                    ImGui::OpenPopup("编辑题目");
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("删除")) {
                    ImGui::OpenPopup("确认删除");
                }

                // --- 编辑弹窗 ---
                static char edit_subject[64] = "";
                static int  edit_type = 2;   // 0=MC, 1=FILL, 2=ESSAY
                static char edit_stem[4096] = "";
                static char edit_option_a[1024] = "";
                static char edit_option_b[1024] = "";
                static char edit_option_c[1024] = "";
                static char edit_option_d[1024] = "";
                static char edit_answer[4096] = "";
                static int  edit_answer_choice = -1; // 选择题答案选择: 0=A,1=B,2=C,3=D
                static char edit_reason[2048] = "";
                static char edit_tags[256] = "";
                static int  edit_mastery = 0;
                static int  editing_id = -1;

                if (ImGui::BeginPopup("编辑题目")) {
                    // 首次打开时从题目数据填充缓冲区
                    if (editing_id != q.id) {
                        editing_id = q.id;
                        strncpy(edit_subject, q.subject.c_str(), sizeof(edit_subject) - 1);
                        edit_type = (q.type == QTYPE_MC) ? 0 : (q.type == QTYPE_FILL) ? 1 : 2;
                        strncpy(edit_stem,    q.stem.c_str(),         sizeof(edit_stem) - 1);
                        strncpy(edit_option_a, q.option_a.c_str(),   sizeof(edit_option_a) - 1);
                        strncpy(edit_option_b, q.option_b.c_str(),   sizeof(edit_option_b) - 1);
                        strncpy(edit_option_c, q.option_c.c_str(),   sizeof(edit_option_c) - 1);
                        strncpy(edit_option_d, q.option_d.c_str(),   sizeof(edit_option_d) - 1);
                        strncpy(edit_answer,  q.answer.c_str(),       sizeof(edit_answer) - 1);
                        edit_answer_choice = (q.answer == "A") ? 0 : (q.answer == "B") ? 1 :
                                             (q.answer == "C") ? 2 : (q.answer == "D") ? 3 : -1;
                        strncpy(edit_reason,  q.error_reason.c_str(), sizeof(edit_reason) - 1);
                        strncpy(edit_tags,    q.tags.c_str(),         sizeof(edit_tags) - 1);
                        edit_mastery = q.mastery;
                    }
                    ImGui::Text("编辑错题 #%d", q.id);
                    ImGui::InputText("科目", edit_subject, sizeof(edit_subject));
                    const char* type_items[] = {"选择题", "填空题", "解答题"};
                    ImGui::Combo("题目类型", &edit_type, type_items, 3);
                    QuestionType editing_type = (edit_type == 0) ? QTYPE_MC :
                                                (edit_type == 1) ? QTYPE_FILL : QTYPE_ESSAY;

                    ImGui::InputTextMultiline("题干", edit_stem, sizeof(edit_stem), ImVec2(-1, 100));

                    if (editing_type == QTYPE_MC) {
                        ImGui::InputText("选项A", edit_option_a, sizeof(edit_option_a));
                        ImGui::InputText("选项B", edit_option_b, sizeof(edit_option_b));
                        ImGui::InputText("选项C", edit_option_c, sizeof(edit_option_c));
                        ImGui::InputText("选项D", edit_option_d, sizeof(edit_option_d));
                    }

                    if (editing_type == QTYPE_MC) {
                        ImGui::Text("正确答案:");
                        const char* choice_labels[] = {"A", "B", "C", "D"};
                        for (int ci = 0; ci < 4; ++ci) {
                            ImGui::SameLine();
                            ImGui::RadioButton(choice_labels[ci], &edit_answer_choice, ci);
                        }
                    } else {
                        ImGui::InputTextMultiline("答案", edit_answer, sizeof(edit_answer), ImVec2(-1, 80));
                    }
                    ImGui::InputTextMultiline("错误原因", edit_reason, sizeof(edit_reason), ImVec2(-1, 60));
                    ImGui::InputText("知识点标签(逗号分隔)", edit_tags, sizeof(edit_tags));
                    ImGui::Text("掌握程度:");
                    for (int m = 0; m <= 5; ++m) {
                        ImGui::SameLine();
                        if (m > 0) { ImGui::SameLine(); }
                        char buf[16];
                        snprintf(buf, sizeof(buf), "%d", m);
                        ImGui::RadioButton(buf, &edit_mastery, m);
                    }
                    ImGui::SameLine();
                    ImGui::TextDisabled("(%s)", mastery_label(edit_mastery));

                    if (ImGui::Button("确定", ImVec2(80, 0))) {
                        const char* choice_letters[] = {"A", "B", "C", "D"};
                        std::string final_answer = (editing_type == QTYPE_MC && edit_answer_choice >= 0)
                            ? choice_letters[edit_answer_choice] : std::string(edit_answer);
                        edit_question(data, q.id, edit_subject, editing_type, edit_stem,
                                      edit_option_a, edit_option_b, edit_option_c, edit_option_d,
                                      final_answer, edit_reason, edit_tags, edit_mastery);
                        editing_id = -1;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("取消", ImVec2(80, 0))) {
                        editing_id = -1;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                // --- 删除确认弹窗 ---
                if (ImGui::BeginPopup("确认删除")) {
                    ImGui::Text("确定要删除错题 #%d 吗？\n此操作不可撤销。", q.id);
                    ImGui::Separator();
                    if (ImGui::Button("确定删除", ImVec2(100, 0))) {
                        delete_question(data, q.id);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("取消", ImVec2(80, 0))) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    // --- 添加新题按钮 ---
    ImGui::Separator();
    float button_width = 150.0f;
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - button_width);
    if (ImGui::Button("+ 添加新题", ImVec2(button_width, 30))) {
        ImGui::OpenPopup("添加错题");
    }

    // --- 添加弹窗 ---
    static char add_subject[64] = "";
    static int  add_type = 2;   // 0=MC, 1=FILL, 2=ESSAY
    static char add_stem[4096] = "";
    static char add_option_a[1024] = "";
    static char add_option_b[1024] = "";
    static char add_option_c[1024] = "";
    static char add_option_d[1024] = "";
    static char add_answer[4096] = "";
    static int  add_answer_choice = -1;
    static char add_reason[2048] = "";
    static char add_tags[256] = "";
    static int  add_mastery = 0;

    if (ImGui::BeginPopupModal("添加错题", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("科目", add_subject, sizeof(add_subject));
        const char* type_items[] = {"选择题", "填空题", "解答题"};
        ImGui::Combo("题目类型", &add_type, type_items, 3);
        QuestionType adding_type = (add_type == 0) ? QTYPE_MC :
                                   (add_type == 1) ? QTYPE_FILL : QTYPE_ESSAY;

        ImGui::InputTextMultiline("题干", add_stem, sizeof(add_stem), ImVec2(500, 120));

        if (adding_type == QTYPE_MC) {
            ImGui::InputText("选项A", add_option_a, sizeof(add_option_a));
            ImGui::InputText("选项B", add_option_b, sizeof(add_option_b));
            ImGui::InputText("选项C", add_option_c, sizeof(add_option_c));
            ImGui::InputText("选项D", add_option_d, sizeof(add_option_d));
        }

        if (adding_type == QTYPE_MC) {
            ImGui::Text("正确答案:");
            const char* add_choice_labels[] = {"A", "B", "C", "D"};
            for (int ci = 0; ci < 4; ++ci) {
                ImGui::SameLine();
                ImGui::RadioButton(add_choice_labels[ci], &add_answer_choice, ci);
            }
        } else {
            ImGui::InputTextMultiline("答案", add_answer, sizeof(add_answer), ImVec2(500, 80));
        }
        ImGui::InputTextMultiline("错误原因", add_reason, sizeof(add_reason), ImVec2(500, 60));
        ImGui::InputText("知识点标签(逗号分隔)", add_tags, sizeof(add_tags));
        ImGui::Text("初始掌握程度:");
        for (int m = 0; m <= 5; ++m) {
            ImGui::SameLine();
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", m);
            ImGui::RadioButton(buf, &add_mastery, m);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", mastery_label(add_mastery));

        ImGui::Separator();
        if (ImGui::Button("确定", ImVec2(120, 0))) {
            const char* add_choice_letters[] = {"A", "B", "C", "D"};
            std::string final_add_answer = (adding_type == QTYPE_MC && add_answer_choice >= 0)
                ? add_choice_letters[add_answer_choice] : std::string(add_answer);
            add_question(data, add_subject, adding_type, add_stem,
                         add_option_a, add_option_b, add_option_c, add_option_d,
                         final_add_answer, add_reason, add_tags, add_mastery);
            // 清空缓冲区
            memset(add_subject,  0, sizeof(add_subject));
            memset(add_stem,     0, sizeof(add_stem));
            memset(add_option_a, 0, sizeof(add_option_a));
            memset(add_option_b, 0, sizeof(add_option_b));
            memset(add_option_c, 0, sizeof(add_option_c));
            memset(add_option_d, 0, sizeof(add_option_d));
            memset(add_answer,   0, sizeof(add_answer));
            memset(add_reason,   0, sizeof(add_reason));
            memset(add_tags,     0, sizeof(add_tags));
            add_mastery = 0;
            add_type = 2;
            add_answer_choice = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("取消", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // --- 设置折叠区 ---
    ImGui::Spacing();
    ImGui::Separator();
    static StudyConfig app_config = load_config();
    static char cfg_api_key[512] = "";
    static char cfg_model_url[512] = "";
    static char cfg_model_name[128] = "";
    static bool cfg_loaded = false;
    if (!cfg_loaded) {
        cfg_loaded = true;
        strncpy(cfg_api_key, app_config.api_key.c_str(), sizeof(cfg_api_key) - 1);
        strncpy(cfg_model_url, app_config.model_url.c_str(), sizeof(cfg_model_url) - 1);
        strncpy(cfg_model_name, app_config.model_name.c_str(), sizeof(cfg_model_name) - 1);
    }

    if (ImGui::CollapsingHeader("设置 (AI 配置)")) {
        ImGui::PushItemWidth(400);
        ImGui::InputText("API Key", cfg_api_key, sizeof(cfg_api_key));
        ImGui::InputText("模型 URL", cfg_model_url, sizeof(cfg_model_url));
        ImGui::InputText("模型名称", cfg_model_name, sizeof(cfg_model_name));
        ImGui::PopItemWidth();
        if (ImGui::Button("保存配置", ImVec2(120, 0))) {
            app_config.api_key = cfg_api_key;
            app_config.model_url = cfg_model_url;
            app_config.model_name = cfg_model_name;
            save_config(app_config);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("配置保存在 studyforge_config.ini");
    }
}

// ============================================================
// 标签页 2：今日复习
// ============================================================

static void render_today_review(AppData& data) {
    static ReviewSession session;

    // 每次渲染都刷新到期题目列表（如果没有活跃会话）
    if (!session.active && !session.completed) {
        session.due_indices = sm2_get_due_indices(data);
        session.current = 0;
        session.showing_answer = false;
        session.selected_quality = -1;
        session.total_reviewed = 0;
        session.mc_selection = -1;
        memset(session.user_response, 0, sizeof(session.user_response));
    }

    ImGui::Text("今日待复习题目: %d 题", (int)session.due_indices.size());
    ImGui::Separator();

    if (session.due_indices.empty()) {
        ImGui::TextDisabled("太棒了！今天没有需要复习的题目。");
        ImGui::TextDisabled("请先添加一些错题吧。");
        return;
    }

    if (!session.active && !session.completed) {
        // 未开始状态
        ImGui::Spacing();
        if (ImGui::Button("开始复习", ImVec2(200, 50))) {
            session.active = true;
            session.current = 0;
            session.showing_answer = false;
            session.selected_quality = -1;
            session.completed = false;
            session.total_reviewed = 0;
            session.mc_selection = -1;
            memset(session.user_response, 0, sizeof(session.user_response));
        }
        return;
    }

    if (session.completed) {
        // 完成状态
        ImGui::Spacing();
        ImGui::Text("复习完成！本论共复习了 %d 道题目。", session.total_reviewed);
        ImGui::Spacing();
        if (ImGui::Button("重新开始", ImVec2(200, 40))) {
            session.active = false;
            session.completed = false;
            session.due_indices = sm2_get_due_indices(data);
        }
        return;
    }

    // --- 复习进行中 ---
    if (session.current >= (int)session.due_indices.size()) {
        // 所有题目已复习完
        session.active = false;
        session.completed = true;
        return;
    }

    int qi = session.due_indices[session.current];
    const auto& q = data.questions[qi];

    // 进度条
    float progress = static_cast<float>(session.current) / session.due_indices.size();
    char progress_text[64];
    snprintf(progress_text, sizeof(progress_text), "%d / %d",
             session.current + 1, (int)session.due_indices.size());
    ImGui::ProgressBar(progress, ImVec2(-1, 0), progress_text);

    ImGui::Spacing();
    ImGui::Text("科目: %s  类型: %s", q.subject.c_str(), question_type_label(q.type));
    if (!q.tags.empty()) {
        ImGui::SameLine();
        ImGui::TextDisabled("  知识点: %s", q.tags.c_str());
    }

    ImGui::Separator();

    // 题干
    bool mc_has_options = (q.type == QTYPE_MC &&
        (!q.option_a.empty() || !q.option_b.empty() || !q.option_c.empty() || !q.option_d.empty()));
    ImGui::Text("题干:");
    ImGui::BeginChild("stem_area", ImVec2(-1, 120), ImGuiChildFlags_Borders);
    ImGui::TextWrapped("%s", q.stem.c_str());
    if (mc_has_options) {
        ImGui::Spacing();
        ImGui::Text("A. %s", q.option_a.c_str());
        ImGui::Text("B. %s", q.option_b.c_str());
        ImGui::Text("C. %s", q.option_c.c_str());
        ImGui::Text("D. %s", q.option_d.c_str());
    }
    ImGui::EndChild();

    if (!session.showing_answer) {
        // --- 阶段1：用户作答 ---
        ImGui::Spacing();
        if (mc_has_options) {
            ImGui::Text("请选择答案:");
            const char* labels[] = {"A", "B", "C", "D"};
            for (int i = 0; i < 4; ++i) {
                ImGui::SameLine();
                char buf[8];
                snprintf(buf, sizeof(buf), "%s", labels[i]);
                ImGui::RadioButton(buf, &session.mc_selection, i);
            }
        } else {
            const char* hint = (q.type == QTYPE_FILL) ? "请输入答案:" : "请作答:";
            ImGui::TextUnformatted(hint);
            ImGui::InputTextMultiline("##user_answer", session.user_response,
                                      sizeof(session.user_response), ImVec2(-1, 80));
        }

        ImGui::Spacing();
        float bw = 200.0f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        if (ImGui::Button("提交答案", ImVec2(bw, 40))) {
            session.showing_answer = true;
        }
    } else {
        // --- 阶段2：展示批改结果 ---
        ImGui::Spacing();

        // 用户答案
        ImGui::Text("你的答案:");
        ImGui::BeginChild("user_resp", ImVec2(-1, 50), ImGuiChildFlags_Borders);
        if (q.type == QTYPE_MC) {
            const char* labels[] = {"A", "B", "C", "D"};
            if (session.mc_selection >= 0 && session.mc_selection < 4) {
                ImGui::Text("%s", labels[session.mc_selection]);
            } else {
                ImGui::TextDisabled("(未选择)");
            }
        } else {
            ImGui::TextWrapped("%s", session.user_response);
        }
        ImGui::EndChild();

        // 正确答案
        ImGui::Text("正确答案:");
        ImGui::BeginChild("correct_area", ImVec2(-1, 80), ImGuiChildFlags_Borders);
        ImGui::TextWrapped("%s", q.answer.c_str());
        ImGui::EndChild();

        // 选择题自动批改提示
        if (q.type == QTYPE_MC && session.mc_selection >= 0) {
            const char* labels[] = {"A", "B", "C", "D"};
            std::string chosen = labels[session.mc_selection];
            bool match = (chosen == q.answer);
            if (match) {
                ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "回答正确!");
            } else {
                ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
                    "回答错误 (你选了 %s, 正确答案是 %s)", chosen.c_str(), q.answer.c_str());
            }
        }

        // 填空题参考匹配提示
        if (q.type == QTYPE_FILL) {
            AutoResult fr = exam_auto_grade(q, session.user_response);
            if (fr == AUTO_CORRECT) {
                ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 0.8f), "(参考: 答案匹配)");
            } else {
                ImGui::TextColored(ImVec4(0.9f, 0.75f, 0.2f, 0.9f), "(参考: 答案不匹配，但可能意思相同，请自行判断)");
            }
        }

        if (!q.error_reason.empty()) {
            ImGui::TextDisabled("错误原因: %s", q.error_reason.c_str());
        }

        ImGui::Separator();
        ImGui::Text("你的回忆程度:");
        ImGui::Spacing();

        const char* quality_labels[] = {
            "0: 完全遗忘", "1: 有印象", "2: 似曾相识",
            "3: 勉强答对", "4: 正确但犹豫", "5: 完美"
        };
        for (int i = 0; i <= 5; ++i) {
            if (i > 0) ImGui::SameLine();
            if (ImGui::Button(quality_labels[i], ImVec2(0, 30))) {
                session.selected_quality = i;
            }
        }

        ImGui::Spacing();
        if (session.selected_quality >= 0) {
            ImGui::Text("已选择: %s", quality_label(session.selected_quality));
            ImGui::Spacing();
            float bw = 200.0f;
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
            if (ImGui::Button("提交并继续", ImVec2(bw, 40))) {
                sm2_update(data.questions[qi], session.selected_quality);
                session.total_reviewed++;
                session.current++;
                session.showing_answer = false;
                session.selected_quality = -1;
                session.mc_selection = -1;
                memset(session.user_response, 0, sizeof(session.user_response));

                if (session.current >= (int)session.due_indices.size()) {
                    session.active = false;
                    session.completed = true;
                }
            }
        }
    }
}

// ============================================================
// 标签页 3：薄弱分析
// ============================================================

static void render_weakness_analysis(const AppData& data) {
    if (data.questions.empty()) {
        ImGui::TextDisabled("请先添加一些错题，才能进行薄弱分析。");
        return;
    }

    // 计算标签统计
    auto tag_stats = compute_tag_stats(data);
    if (tag_stats.empty()) {
        ImGui::TextDisabled("没有找到知识点标签。");
        return;
    }

    sort_by_health(tag_stats);

    ImGui::Text("知识点健康度排行（越低越薄弱）");
    ImGui::Separator();
    ImGui::Spacing();

    // 柱状图：每个标签一行
    for (const auto& stat : tag_stats) {
        ImGui::PushID(stat.tag.c_str());

        // 标签名和题目数
        ImGui::Text("%-16s (%d题)", stat.tag.c_str(), stat.question_count);
        ImGui::SameLine(220);

        // 颜色编码：红 < 2, 黄 < 3.5, 绿 >= 3.5
        ImVec4 bar_color;
        if (stat.health_score < 2.0f) {
            bar_color = ImVec4(0.9f, 0.3f, 0.3f, 1.0f);   // 红色
        } else if (stat.health_score < 3.5f) {
            bar_color = ImVec4(0.9f, 0.75f, 0.2f, 1.0f);   // 黄色
        } else {
            bar_color = ImVec4(0.3f, 0.85f, 0.3f, 1.0f);   // 绿色
        }

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, bar_color);
        float v = stat.health_score;
        char label[32];
        snprintf(label, sizeof(label), "%.1f", stat.health_score);
        ImGui::PlotHistogram("##health", &v, 1, 0.0f, label, 0.0f, 5.0f, ImVec2(250, 20));
        ImGui::PopStyleColor();

        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // 标签共现分析（关联薄弱区）
    auto co_occurrences = compute_tag_co_occurrence(data);
    if (!co_occurrences.empty()) {
        ImGui::Text("关联薄弱区分析:");
        ImGui::Spacing();
        int show_count = std::min((int)co_occurrences.size(), 10);
        for (int i = 0; i < show_count; ++i) {
            const auto& c = co_occurrences[i];
            ImGui::Text("  \"%s\" 和 \"%s\" 经常同时出错 (%d题, 薄弱度%.1f)",
                        c.tag_a.c_str(), c.tag_b.c_str(), c.count, c.combined_weakness);
        }
    } else {
        ImGui::TextDisabled("暂无关联薄弱区数据（需要标签含2个以上的薄弱题）。");
    }
}

// ============================================================
// 标签页 4：一键组卷
// ============================================================

static void render_exam_generator(AppData& data) {
    static ExamSession exam;
    static char exam_user_response[4096] = "";
    static int  exam_mc_selection = -1;
    static int  exam_subject_idx = 0;
    static int  exam_count = 10;

    if (data.questions.empty()) {
        ImGui::TextDisabled("没有可用的题目，请先添加错题。");
        return;
    }

    auto subjects = get_all_subjects(data);
    static std::string exam_combo_str;

    if (exam.phase == ExamSession::CONFIG) {
        // --- 配置阶段 ---
        exam_combo_str = "全部";
        for (const auto& s : subjects) { exam_combo_str += '\0'; exam_combo_str += s; }
        exam_combo_str += '\0';

        ImGui::Text("组卷设置");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushItemWidth(200);
        ImGui::Combo("科目", &exam_subject_idx, exam_combo_str.c_str());
        ImGui::InputInt("题目数量", &exam_count);
        ImGui::PopItemWidth();

        if (exam_count < 1) exam_count = 1;
        if (exam_count > 200) exam_count = 200;

        ImGui::Spacing();
        if (ImGui::Button("开始考试", ImVec2(200, 35))) {
            std::string subject = (exam_subject_idx == 0) ? "" : subjects[exam_subject_idx - 1];
            exam_start(exam, data, subject, exam_count);
            if (exam.selected_indices.empty()) {
                exam.phase = ExamSession::CONFIG;
            }
            exam_mc_selection = -1;
            memset(exam_user_response, 0, sizeof(exam_user_response));
        }

        // 历史考试记录
        if (!data.exam_history.empty()) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("历次考试记录:");
            ImGui::Spacing();
            if (ImGui::BeginTable("exam_history", 4,
                                  ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV,
                                  ImVec2(0, 0))) {
                ImGui::TableSetupColumn("日期",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("科目",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("成绩",    ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("详情",    ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int i = (int)data.exam_history.size() - 1; i >= 0; --i) {
                    const auto& rec = data.exam_history[i];
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(rec.date.to_string().c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(rec.subject.empty() ? "全部" : rec.subject.c_str());
                    ImGui::TableSetColumnIndex(2);
                    if (rec.graded > 0) {
                        ImGui::Text("%d/%d", rec.correct, rec.graded);
                    } else {
                        ImGui::TextDisabled("--");
                    }
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d题", rec.total);
                }
                ImGui::EndTable();
            }
        }

        return;
    }

    if (exam.phase == ExamSession::QUIZZING) {
        // --- 作答阶段 ---
        if (exam.current >= (int)exam.selected_indices.size()) {
            exam_compute_summary(exam, data);
            exam.phase = ExamSession::GRADING;
            return;
        }

        int qi = exam.selected_indices[exam.current];
        const auto& q = data.questions[qi];

        float prog = static_cast<float>(exam.current) / exam.selected_indices.size();
        char ptext[64];
        snprintf(ptext, sizeof(ptext), "%d / %d", exam.current + 1, (int)exam.selected_indices.size());
        ImGui::ProgressBar(prog, ImVec2(-1, 0), ptext);

        ImGui::Spacing();
        ImGui::Text("科目: %s  类型: %s", q.subject.c_str(), question_type_label(q.type));
        if (!q.tags.empty()) {
            ImGui::SameLine();
            ImGui::TextDisabled("  知识点: %s", q.tags.c_str());
        }
        ImGui::Separator();

        // 题干
        bool exam_mc_has = (q.type == QTYPE_MC &&
            (!q.option_a.empty() || !q.option_b.empty() || !q.option_c.empty() || !q.option_d.empty()));
        ImGui::BeginChild("exam_stem", ImVec2(-1, 120), ImGuiChildFlags_Borders);
        ImGui::TextWrapped("%s", q.stem.c_str());
        if (exam_mc_has) {
            ImGui::Spacing();
            ImGui::Text("A. %s", q.option_a.c_str());
            ImGui::Text("B. %s", q.option_b.c_str());
            ImGui::Text("C. %s", q.option_c.c_str());
            ImGui::Text("D. %s", q.option_d.c_str());
        }
        ImGui::EndChild();

        // 作答区
        if (exam_mc_has) {
            ImGui::Text("请选择答案:");
            const char* labels[] = {"A", "B", "C", "D"};
            for (int i = 0; i < 4; ++i) {
                ImGui::SameLine();
                char buf[8];
                snprintf(buf, sizeof(buf), "%s", labels[i]);
                ImGui::RadioButton(buf, &exam_mc_selection, i);
            }
        } else {
            const char* hint = (q.type == QTYPE_FILL) ? "请输入答案:" : "请作答:";
            ImGui::TextUnformatted(hint);
            ImGui::InputTextMultiline("##exam_answer", exam_user_response,
                                      sizeof(exam_user_response), ImVec2(-1, 80));
        }

        ImGui::Spacing();
        float bw = 200.0f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f);
        if (ImGui::Button("下一题", ImVec2(bw, 40))) {
            auto& ans = exam.answers[exam.current];
            if (q.type == QTYPE_MC) {
                const char* labels[] = {"A", "B", "C", "D"};
                if (exam_mc_selection >= 0) ans.user_response = labels[exam_mc_selection];
            } else {
                ans.user_response = exam_user_response;
            }
            exam.current++;
            exam_mc_selection = -1;
            memset(exam_user_response, 0, sizeof(exam_user_response));
        }
        ImGui::SameLine();
        if (ImGui::Button("结束考试", ImVec2(120, 40))) {
            auto& ans = exam.answers[exam.current];
            if (q.type == QTYPE_MC) {
                const char* labels[] = {"A", "B", "C", "D"};
                if (exam_mc_selection >= 0) ans.user_response = labels[exam_mc_selection];
            } else {
                ans.user_response = exam_user_response;
            }
            exam_compute_summary(exam, data);
            exam.phase = ExamSession::GRADING;
        }
        return;
    }

    if (exam.phase == ExamSession::GRADING) {
        // --- 评分阶段 ---
        int total = (int)exam.answers.size();
        int graded = 0, correct = 0;
        for (const auto& a : exam.answers) {
            if (a.self_grade >= 0) { graded++; if (a.self_grade == 1) correct++; }
        }

        ImGui::Text("考试结果");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("总题数: %d   已评: %d   正确: %d", total, graded, correct);
        if (graded > 0) {
            float score = 100.0f * correct / graded;
            char score_text[32];
            snprintf(score_text, sizeof(score_text), "%.0f 分", score);
            ImGui::ProgressBar(score / 100.0f, ImVec2(-1, 0), score_text);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("逐题详情:");
        ImGui::Spacing();

        if (ImGui::BeginTable("exam_results", 4,
                              ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV,
                              ImVec2(0, ImGui::GetContentRegionAvail().y - 50))) {
            ImGui::TableSetupColumn("#",     ImGuiTableColumnFlags_WidthFixed, 30.0f);
            ImGui::TableSetupColumn("类型",  ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("结果",  ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("详情",  ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (int i = 0; i < total; ++i) {
                auto& ans = exam.answers[i];
                const auto& q = data.questions[ans.question_index];
                ImGui::TableNextRow();
                ImGui::PushID(i);

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i + 1);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(question_type_label(q.type));

                ImGui::TableSetColumnIndex(2);
                if (q.type == QTYPE_MC) {
                    // 选择题：自动批改，不可改
                    if (ans.is_correct) {
                        ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "正确");
                    } else {
                        ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "错误");
                    }
                } else {
                    // 填空题/解答题：自评按钮
                    if (q.type == QTYPE_FILL) {
                        if (ans.auto_result == AUTO_CORRECT) {
                            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 0.7f), "(参考:匹配)");
                        } else {
                            ImGui::TextColored(ImVec4(0.9f, 0.75f, 0.2f, 0.9f), "(参考:不匹配)");
                        }
                    }
                    if (ans.self_grade == 1) {
                        ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "自评:正确");
                    } else if (ans.self_grade == 0) {
                        ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "自评:错误");
                    } else {
                        if (ImGui::SmallButton("正确")) ans.self_grade = 1;
                        ImGui::SameLine();
                        if (ImGui::SmallButton("错误")) ans.self_grade = 0;
                    }
                }

                ImGui::TableSetColumnIndex(3);
                ImGui::TextWrapped("%s", stem_preview(q.stem, 30).c_str());
                ImGui::TextDisabled("你答: %s", ans.user_response.empty() ? "(未作答)" : ans.user_response.c_str());
                ImGui::TextDisabled("正确: %s", q.answer.c_str());

                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        if (ImGui::Button("返回组卷", ImVec2(200, 35))) {
            // 保存考试历史记录
            ExamRecord rec;
            rec.date = get_today();
            rec.subject = exam.subject;
            rec.total = (int)exam.answers.size();
            rec.correct = correct;
            rec.graded = graded;
            data.exam_history.push_back(rec);
            exam = ExamSession();
        }
    }
}

// ============================================================
// main — 程序入口
// ============================================================

int main(int argc, char** argv) {
    // --- 1. 初始化 GLFW ---
    if (!glfwInit()) {
        fprintf(stderr, "无法初始化 GLFW\n");
        return 1;
    }

    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720,
        "\xe5\xad\xa6\xe9\x9c\xb8\xe9\x94\x99\xe9\xa2\x98\xe5\x8a\xa9\xe6\x89\x8b", // 学霸错题助手
        nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "无法创建 GLFW 窗口\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // --- 2. 加载 GLAD ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "无法初始化 GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // --- 3. 初始化 ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // --- 4. 加载中文字体 ---
    ImFontConfig font_config;
    font_config.OversampleH = 1;
    font_config.OversampleV = 1;
    ImFont* font = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\msyh.ttc", 26.0f,
        &font_config, io.Fonts->GetGlyphRangesChineseFull());
    if (!font) {
        // 备选：尝试 msyh.ttf
        font = io.Fonts->AddFontFromFileTTF(
            "C:\\Windows\\Fonts\\msyh.ttf", 26.0f,
            &font_config, io.Fonts->GetGlyphRangesChineseFull());
    }
    if (!font) {
        fprintf(stderr, "警告: 无法加载中文字体，使用默认字体\n");
        io.Fonts->AddFontDefault();
    }
    // --- 5. 设置 ImGui 风格 ---
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScaleAllSizes(1.4f);

    // --- 6. 初始化 ImGui 后端 ---
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --- 7. 加载数据 ---
    std::string data_path = get_data_filepath();
    AppData data = load_data(data_path);

    // --- 8. 主循环 ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // ImGui 新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 主窗口（全屏）
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##Main", nullptr,
                      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                      ImGuiWindowFlags_NoBringToFrontOnFocus);

        // 标题
        ImGui::Text("学霸错题助手");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 200);
        ImGui::TextDisabled("共 %d 道错题", (int)data.questions.size());
        ImGui::Separator();

        // 标签页栏
        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("错题管理")) {
                render_question_management(data);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("今日复习")) {
                render_today_review(data);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("薄弱分析")) {
                render_weakness_analysis(data);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("一键组卷")) {
                render_exam_generator(data);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // --- 渲染 ---
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // --- 9. 保存数据 ---
    save_data(data, data_path);

    // --- 10. 清理 ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
