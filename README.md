# study-planner-c

这是一个简单的 C 语言命令行学习计划工具，适合用来练习 C 语言基础、结构体、数组、菜单交互和文件读写。

当前版本：`v1.5`

## 项目简介

程序可以用来管理学习任务。你可以添加任务、查看任务、删除任务、标记任务完成，并把任务保存到本地文件中。下次重新运行程序时，会自动读取之前保存的任务。

`v1.1` 增加了任务优先级功能，使用 `H / M / L` 显示优先级：

```text
H = High
M = Medium
L = Low
```

`v1.2` 增加了任务截止日期功能，日期格式为：

```text
MM-DD
```

例如：

```text
06-20
12-31
```

`v1.3` 增加了任务显示排序功能。查看任务时，程序会按照下面的规则排序：

```text
1. 优先级从高到低：H -> M -> L
2. 同优先级按截止日期从近到远排序
```

`v1.4` 增加了搜索功能。输入关键词后，程序会单独显示所有标题中包含该关键词的任务。

`v1.5` 修复了搜索和其他功能之间的编号联动问题。现在查看任务和搜索任务时显示的都是任务真实编号，删除、标记完成、修改截止日期等操作也统一使用真实编号。

## 功能列表

- 添加学习任务
- 添加任务时设置优先级
- 添加任务时输入截止日期
- 查看所有任务
- 查看任务完成状态、优先级和截止日期
- 查看任务时自动按优先级和截止日期排序
- 按关键词搜索任务
- 搜索结果显示任务真实编号
- 修改任务截止日期并保存
- 删除指定任务
- 标记任务为已完成
- 保存任务到本地文件
- 启动程序时自动读取已保存任务
- 对错误菜单编号和错误任务编号做基本处理

## 项目结构

```text
study-planner-c/
  .gitignore
  Makefile
  README.md
  src/
    main.c
```

主要文件说明：

```text
src/main.c      程序源代码
Makefile        使用 GCC/make 编译时的构建文件
README.md       项目说明文档
.gitignore      Git 忽略规则
```

运行程序后会自动生成：

```text
tasks.txt
```

`tasks.txt` 用来保存本地任务数据，不需要提交到 GitHub。`.exe`、`.obj`、`.o` 等编译产物也已经被 `.gitignore` 忽略。

## Windows 下使用 Visual Studio 编译器编译

本项目可以使用 Visual Studio 自带的 Microsoft C/C++ 编译器编译。

### 方法一：使用 VS 2022 开发者命令提示符

从开始菜单打开：

```text
x64 Native Tools Command Prompt for VS 2022
```

进入仓库根目录：

```bat
cd /d "D:\Program Files\codex_program\first-program"
```

编译：

```bat
cl src\main.c /Fe:study_planner.exe
```

编译成功后会生成：

```text
study_planner.exe
```

### 方法二：在普通 CMD 中手动加载编译环境

如果你打开的是普通 CMD，需要先执行：

```bat
call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat"
```

然后进入仓库根目录并编译：

```bat
cd /d "D:\Program Files\codex_program\first-program"
cl src\main.c /Fe:study_planner.exe
```

## 可选：使用 GCC 编译

如果你安装了 MSYS2、MinGW-w64 或其他 GCC 环境，也可以使用：

```sh
gcc -Wall -Wextra -std=c11 src/main.c -o study_planner.exe
```

如果安装了 `make`，可以直接运行：

```sh
make
```

## 如何运行

在仓库根目录运行：

```bat
study_planner.exe
```

如果使用 PowerShell，运行：

```powershell
.\study_planner.exe
```

## 使用示例

运行程序后，会看到菜单：

```text
==== Study Planner C ====
1. Add task
2. View all tasks
3. Delete task
4. Mark task as done
5. Update due date
6. Search tasks
7. Save tasks
8. Exit
Choose an option:
```

添加任务时，会要求输入任务名、优先级和截止日期：

```text
Choose an option: 1
Enter task title: Review C arrays
Priority options: 1=High, 2=Medium, 3=Low
Enter priority: 1
Enter due date (MM-DD): 06-20
Task added.
```

查看任务时，会显示完成状态、优先级和截止日期。任务会自动按照优先级和截止日期排序：

```text
Choose an option: 2
Tasks:
3. [ ] [H] [06-20] Review C arrays
2. [ ] [M] [06-18] Read textbook
1. [ ] [L] [06-15] Organize notes
```

注意：任务前面的数字是真实编号，不是临时排序编号。删除、标记完成、修改截止日期时，都要输入这个真实编号。

修改截止日期：

```text
Choose an option: 5
Tasks:
3. [ ] [H] [06-20] Review C arrays
Enter task number to update due date: 3
Enter due date (MM-DD): 06-25
Due date updated.
```

搜索任务：

```text
Choose an option: 6
Enter search keyword: C

Search results:
3. [ ] [H] [06-20] Review C arrays
```

再次查看：

```text
Choose an option: 2
Tasks:
3. [ ] [H] [06-25] Review C arrays
```

退出：

```text
Choose an option: 8
Goodbye!
```

再次运行程序时，之前保存的任务会从 `tasks.txt` 自动读取。

## 测试建议

重新编译后，可以按下面流程测试 `v1.5`：

```text
1. 添加 Low 任务，优先级选择 3，截止日期输入 01-01
2. 添加 High 任务，优先级选择 1，截止日期输入 12-01
3. 添加 High 任务，优先级选择 1，截止日期输入 02-01
4. 添加 Medium 任务，优先级选择 2，截止日期输入 03-01
5. 查看任务，确认显示顺序为：
   3. [H] [02-01]
   2. [H] [12-01]
   4. [M] [03-01]
   1. [L] [01-01]
6. 搜索关键词 High，确认搜索结果显示真实编号 3 和 2
7. 输入真实编号 3 标记完成，确认改的是编号 3 的任务
8. 输入真实编号 2 修改截止日期，确认改的是编号 2 的任务
9. 输入不存在的编号，例如 99，确认提示 Invalid task number.
10. 删除测试任务时也使用真实编号
```

## 后续可改进方向

- 支持编辑任务标题
- 支持把已完成任务改回未完成
- 把菜单文字改成中文
- 改进不同 Windows 终端下的中文编码显示
- 使用 CSV 或 JSON 等更结构化的方式保存任务
