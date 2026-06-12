# study-planner-c

这是一个简单的 C 语言命令行学习计划工具，适合用来练习 C 语言基础、结构体、数组、菜单交互和文件读写。

当前版本：`v1.2`

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

## 功能列表

- 添加学习任务
- 添加任务时设置优先级
- 添加任务时输入截止日期
- 查看所有任务
- 查看任务完成状态、优先级和截止日期
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
6. Save tasks
7. Exit
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

查看任务时，会显示完成状态、优先级和截止日期：

```text
Choose an option: 2
Tasks:
1. [ ] [H] [06-20] Review C arrays
```

修改截止日期：

```text
Choose an option: 5
Tasks:
1. [ ] [H] [06-20] Review C arrays
Enter task number to update due date: 1
Enter due date (MM-DD): 06-25
Due date updated.
```

再次查看：

```text
Choose an option: 2
Tasks:
1. [ ] [H] [06-25] Review C arrays
```

退出：

```text
Choose an option: 7
Goodbye!
```

再次运行程序时，之前保存的任务会从 `tasks.txt` 自动读取。

## 测试建议

重新编译后，可以按下面流程测试 `v1.2`：

```text
1. 添加一个任务，优先级选择 1，截止日期输入 06-20
2. 查看任务，确认显示 [H] [06-20]
3. 修改这个任务的截止日期为 06-25
4. 再次查看任务，确认显示 [H] [06-25]
5. 退出程序
6. 再次运行程序，确认任务、优先级和截止日期都能读取回来
7. 删除测试任务
```

## 后续可改进方向

- 支持编辑任务标题
- 支持把已完成任务改回未完成
- 支持按优先级或截止日期排序任务
- 支持搜索或筛选任务
- 把菜单文字改成中文
- 改进不同 Windows 终端下的中文编码显示
- 使用 CSV 或 JSON 等更结构化的方式保存任务
