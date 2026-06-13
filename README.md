# study-planner-c

当前版本：`v2.4`

一个使用 C 语言编写的 Windows 命令行学习任务管理工具。

v2.4 是桌面版原型前的最后一个命令行整理版本。这个版本不增加新功能，重点是让任务数据结构、保存读取格式、核心模块边界和测试清单都稳定下来，方便以后继续开发桌面 GUI 或迁移到 SQLite。

## 项目简介

`study-planner-c` 用来在命令行中管理学习任务。程序支持添加、查看、搜索、修改、删除、标记完成、清空、保存和自动读取任务。

项目不使用复杂第三方库，适合练习 C 语言中的结构体、数组、字符串处理、文件读写、模块拆分和 Windows 编译运行。

## 当前功能

- 添加学习任务
- 设置任务优先级：`H / M / L`
- 设置截止日期：`MM-DD`
- 查看所有任务
- 按优先级从高到低显示任务
- 同优先级按日期从近到远显示任务
- 按关键词搜索任务
- 搜索结果显示全局真实任务编号
- 从任务列表或搜索结果进入任务操作菜单
- 标记完成 / 取消完成
- 修改任务名称
- 修改任务优先级
- 修改任务截止日期
- 删除单个任务，删除前需要确认
- 清空全部任务，清空前需要确认
- 保存任务到本地文件
- 下次启动时自动读取任务
- 中文菜单、中文提示和中文任务名称

## v2.4 状态

v2.4 不增加新功能，主要完成以下整理：

- 稳定任务数据结构
- 稳定保存读取格式
- 保持旧格式读取兼容
- 明确核心模块职责
- 保持命令行界面和核心逻辑分离
- 补全 README 和完整测试清单

## 任务数据结构

核心任务结构定义在 `src/task.h`：

```c
typedef struct {
    char title[MAX_TITLE];
    char due_date[DATE_LEN];
    int done;
    int priority;
} Task;
```

字段说明：

- `title`：任务名称，最多 `MAX_TITLE - 1` 个字符
- `due_date`：截止日期，格式为 `MM-DD`
- `done`：完成状态
- `priority`：任务优先级

状态常量：

```c
TASK_NOT_DONE = 0
TASK_DONE     = 1
```

优先级常量：

```c
TASK_PRIORITY_HIGH   = 1
TASK_PRIORITY_MEDIUM = 2
TASK_PRIORITY_LOW    = 3
```

命令行显示时：

- `1` 显示为 `H`
- `2` 显示为 `M`
- `3` 显示为 `L`

## 保存格式

任务保存在项目根目录的 `tasks.txt` 中。

v2.4 使用稳定文本格式：

```text
# study-planner-c tasks v2
# fields: record_type|done|priority|due_date|title
TASK|0|3|06-15|复习 C 语言数组
```

字段说明：

- `record_type`：记录类型，目前固定为 `TASK`
- `done`：完成状态，`0` 未完成，`1` 已完成
- `priority`：优先级，`1` 高，`2` 中，`3` 低
- `due_date`：截止日期，格式为 `MM-DD`
- `title`：任务名称

为了避免任务名称破坏保存格式，`title` 中的特殊字符会被转义：

- `|` 保存为 `\|`
- `\` 保存为 `\\`
- 换行会保存为 `\n` 或 `\r`

程序仍兼容读取旧格式：

```text
0|3|06-15|复习 C 语言数组
```

旧格式读取后，再保存会自动写成新的 v2 格式。

## 项目结构

```text
study-planner-c/
  .gitignore
  Makefile
  README.md
  tasks.txt
  src/
    main.c
    task.c
    task.h
    storage.c
    storage.h
    cli_ui.c
    cli_ui.h
```

文件说明：

```text
src/main.c      程序入口，负责启动主流程
src/task.c      任务增删改查、搜索、筛选、排序和校验
src/task.h      任务核心模块头文件
src/storage.c   任务保存和读取
src/storage.h   存储模块头文件
src/cli_ui.c    命令行菜单、输入、输出和交互流程
src/cli_ui.h    命令行界面模块头文件
Makefile        GCC/make 构建文件
README.md       项目说明文档
.gitignore      Git 忽略规则
tasks.txt       本地任务数据文件
```

## 模块职责

### `task.c / task.h`

只负责任务核心逻辑：

- 添加任务
- 删除任务
- 修改任务
- 标记完成
- 清空任务
- 搜索任务
- 筛选任务
- 排序任务
- 校验优先级和日期

这个模块不负责命令行输入输出，也不负责文件读写。

### `storage.c / storage.h`

只负责保存和读取：

- 从 `tasks.txt` 读取任务
- 保存任务到 `tasks.txt`
- 解析新格式 `TASK|...`
- 兼容读取旧格式
- 对任务名称中的特殊字符做转义和反转义

这个模块不负责菜单，也不直接处理用户输入。

### `cli_ui.c / cli_ui.h`

只负责命令行界面：

- 显示菜单
- 读取用户输入
- 打印任务列表
- 打印搜索结果
- 调用 `task.c` 的核心逻辑
- 调用 `storage.c` 保存任务

以后做桌面 GUI 时，可以新增一套 GUI 界面层，复用 `task.c` 和 `storage.c`。

### `main.c`

只负责程序启动：

- 设置 Windows 控制台 UTF-8 编码
- 调用 `load_tasks()`
- 启动命令行界面

## Windows 下如何编译

推荐使用 Visual Studio 自带的 Microsoft C/C++ 编译器。

### 方法一：使用 VS 2022 开发者命令提示符

从开始菜单打开：

```text
x64 Native Tools Command Prompt for VS 2022
```

进入项目根目录：

```bat
cd /d "D:\Program Files\codex_program\first-program"
```

编译：

```bat
cl /utf-8 src\main.c src\task.c src\storage.c src\cli_ui.c /Fe:study_planner.exe
```

### 方法二：在普通 CMD 中手动加载编译环境

```bat
call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat"
cd /d "D:\Program Files\codex_program\first-program"
cl /utf-8 src\main.c src\task.c src\storage.c src\cli_ui.c /Fe:study_planner.exe
```

### 方法三：使用 Makefile

如果已经安装 GCC 和 make，可以运行：

```bat
make
```

## 如何运行

在项目根目录运行：

```bat
study_planner.exe
```

PowerShell 中运行：

```powershell
.\study_planner.exe
```

## 使用示例

主菜单：

```text
==== 学习任务计划 ====
1. 添加任务
2. 查看所有任务
3. 搜索任务
4. 清空全部任务
5. 保存任务
6. 退出
请选择操作:
```

添加任务：

```text
请选择操作: 1
请输入任务名称: 复习 C 语言数组
优先级选项：1=高, 2=中, 3=低
请输入优先级: 1
请输入截止日期 (MM-DD): 06-20
任务已添加。
```

查看任务：

```text
请选择操作: 2

任务列表:
3. [ ] [H] [06-20] 复习 C 语言数组
2. [ ] [M] [06-18] 阅读教材
1. [ ] [L] [06-15] 整理笔记
请输入要操作的任务编号，输入 0 返回:
```

任务前面的数字是真实编号，不是临时排序编号。删除、修改、标记完成都使用这个真实编号。

## 完整测试清单

### 1. 编译测试

- 执行 Visual Studio 编译命令
- 确认生成 `study_planner.exe`
- 确认没有编译错误

### 2. 启动读取测试

- 运行 `.\study_planner.exe`
- 确认菜单能正常显示中文
- 确认已有任务能自动读取
- 选择 `6` 能正常退出

### 3. 添加任务测试

- 添加一个英文任务
- 添加一个中文任务
- 添加高优先级任务
- 添加中优先级任务
- 添加低优先级任务
- 输入空任务名，确认提示错误
- 输入错误优先级，例如 `9`，确认使用默认中优先级
- 输入错误日期，例如 `02-31`，确认要求重新输入

### 4. 查看和排序测试

- 选择 `2. 查看所有任务`
- 确认任务显示格式为：编号、完成状态、优先级、日期、任务名
- 确认优先级按 `H -> M -> L` 排序
- 确认同优先级按日期从近到远排序
- 在任务列表输入不存在编号，例如 `999`，确认提示错误
- 在任务列表输入 `0`，确认返回主菜单

### 5. 任务操作菜单测试

- 从任务列表输入真实任务编号
- 测试标记完成
- 再次测试取消完成
- 测试修改任务名称
- 测试修改优先级
- 测试修改截止日期
- 在任务操作菜单输入错误选项，确认不会崩溃
- 选择 `6. 返回`，确认回到任务列表

### 6. 搜索测试

- 选择 `3. 搜索任务`
- 输入中文关键词
- 输入英文关键词
- 输入不存在的关键词，确认提示没有结果
- 输入空关键词，确认提示错误
- 确认搜索结果显示真实任务编号
- 从搜索结果输入任务编号进入操作菜单
- 选择 `6. 返回`，确认回到搜索结果
- 在搜索结果输入不属于搜索结果的编号，确认提示错误

### 7. 删除测试

- 从任务列表进入任务操作菜单
- 选择删除任务
- 输入 `n`，确认取消删除
- 再次选择删除任务
- 输入 `y`，确认任务被删除
- 删除后确认编号仍然正常

### 8. 清空测试

- 选择 `4. 清空全部任务`
- 输入 `no`，确认取消清空
- 再次选择清空
- 输入 `yes`，确认全部任务被清空
- 清空后查看任务，确认提示还没有任务

### 9. 保存读取测试

- 添加几条任务
- 选择 `5. 保存任务`
- 退出程序
- 重新运行程序
- 确认任务仍然存在
- 修改任务后退出
- 再次运行，确认修改内容仍然存在

### 10. 保存格式测试

- 打开 `tasks.txt`
- 确认前两行是格式说明：

```text
# study-planner-c tasks v2
# fields: record_type|done|priority|due_date|title
```

- 确认任务行以 `TASK|` 开头
- 添加名称包含 `|` 的任务，例如 `A|B`
- 保存并重新运行
- 确认任务名称仍显示为 `A|B`

### 11. 非法输入稳定性测试

- 主菜单输入 `abc`
- 主菜单输入 `2abc`
- 主菜单输入超长内容
- 日期输入 `13-01`
- 日期输入 `04-31`
- 日期输入空内容
- 任务编号输入不存在编号
- 确认程序不会崩溃

## Git 忽略规则

以下文件不需要提交到 GitHub：

- `study_planner.exe`
- `*.obj`
- `*.o`
- `tasks.txt`

这些编译产物和本地数据文件应该由 `.gitignore` 忽略。

## 后续计划

- 开发桌面 GUI 原型
- 让 GUI 复用 `task.c` 和 `storage.c`
- 将 `tasks.txt` 迁移到 SQLite
- 为 SQLite 设计任务表结构
- 增加自动化测试脚本
