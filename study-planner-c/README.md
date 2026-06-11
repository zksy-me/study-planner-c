# study-planner-c

`study-planner-c` is a simple command-line study task planner written in C.

It is designed as a beginner-friendly Windows C project. The code uses only the C standard library and stores task data in a local text file.

## Features

- Add a study task
- View all study tasks
- Delete a task
- Mark a task as done
- Save tasks to a local file
- Automatically load saved tasks when the program starts

## Project Structure

```text
study-planner-c/
  .gitignore
  Makefile
  README.md
  src/
    main.c
```

Runtime files:

```text
tasks.txt
```

`tasks.txt` is created automatically when tasks are saved. It is ignored by Git because it stores local user data.

Build output files such as `.exe`, `.obj`, and `.o` are also ignored by Git.

## Compile on Windows with Visual Studio Compiler

This project can be compiled with the Microsoft C/C++ compiler that comes with Visual Studio.

### Option 1: Use the Visual Studio Developer Command Prompt

Open this from the Start Menu:

```text
x64 Native Tools Command Prompt for VS 2022
```

Then go to the project folder:

```bat
cd /d "D:\Program Files\codex_program\first-program\study-planner-c"
```

Compile:

```bat
cl src\main.c /Fe:study_planner.exe
```

This creates:

```text
study_planner.exe
```

### Option 2: Use a Normal Command Prompt

If you are using a normal Command Prompt, first load the Visual Studio compiler environment:

```bat
call "D:\visual studio\VC\Auxiliary\Build\vcvars64.bat"
```

Then compile:

```bat
cd /d "D:\Program Files\codex_program\first-program\study-planner-c"
cl src\main.c /Fe:study_planner.exe
```

## Optional GCC Build

If you have GCC installed, such as through MSYS2 or MinGW-w64, you can also build with:

```sh
gcc -Wall -Wextra -std=c11 src/main.c -o study_planner.exe
```

If `make` is installed:

```sh
make
```

## Run

In the project folder, run:

```bat
study_planner.exe
```

If you are using PowerShell, you may need:

```powershell
.\study_planner.exe
```

## Usage Example

After running the program, you will see a menu:

```text
==== Study Planner C ====
1. Add task
2. View all tasks
3. Delete task
4. Mark task as done
5. Save tasks
6. Exit
Choose an option:
```

Example flow:

```text
Choose an option: 1
Enter task title: Review C arrays
Task added.

Choose an option: 2
Tasks:
1. [ ] Review C arrays

Choose an option: 4
Enter task number to mark as done: 1
Task marked as done.

Choose an option: 2
Tasks:
1. [x] Review C arrays

Choose an option: 6
Goodbye!
```

When you open the program again, saved tasks are loaded from `tasks.txt`.

## Clean Build Files

If you built with `make`, you can remove the generated executable with:

```sh
make clean
```

This removes `study_planner.exe`. It does not remove `tasks.txt`.

## Future Improvements

- Add task due dates
- Add task priority levels
- Support editing an existing task
- Support marking a completed task as not done
- Add search or filter options
- Improve Chinese text display support across different Windows terminal encodings
- Store tasks in a more structured format, such as CSV or JSON
