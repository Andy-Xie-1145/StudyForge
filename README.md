# StudyForge — 学霸错题助手

一款基于 **SM-2 间隔重复算法** 的桌面错题管理工具，帮助初中生高效整理、复习错题。

## ✨ 功能

- **错题录入**：支持选择题、填空题、解答题，按科目/标签分类
- **智能复习**：基于 SM-2 算法自动安排复习计划，到期题目优先推送
- **标签分析**：统计各科目/标签的错题分布与关联，定位薄弱点
- **模拟试卷**：按紧急度生成复习试卷（纯文本）
- **AI 辅助**：预留 AI 评分接口（OpenAI 兼容 API），可接入大模型自动批改解答题

## 🛠️ 技术栈

- **语言**：C++17
- **GUI**：Dear ImGui + GLFW + OpenGL 3.3
- **算法**：SM-2 间隔重复
- **构建**：MinGW-w64（Windows）

## 📦 构建

```bash
build_mingw.bat
```

产物：`build/studyforge.exe`

## 📄 项目结构

```
main.cpp          → UI 层（4 个 ImGui 标签页，GLFW/OpenGL 初始化）
models.h          → 数据模型（SimpleDate、Question、AppData）
crud.h            → 增删改查操作
sm2_engine.h      → SM-2 间隔重复算法
analytics.h       → 标签统计与关联分析
exam.h            → 模拟试卷生成
persistence.h     → 数据持久化（.dat 文本格式）
config.h          → 配置读写
ai.h              → AI 辅助评判（预留接口）
```

## 📝 作者

谢言睿 — 初中生
