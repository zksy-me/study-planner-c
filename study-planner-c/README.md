# study-planner-c

A simple C command-line study planner for Windows.

## Features

- Add study tasks
- View all tasks
- Delete tasks
- Mark tasks as done
- Save tasks to a local file
- Automatically load saved tasks when the program starts

## Project Structure

```text
study-planner-c/
  Makefile
  README.md
  src/
    main.c
```

The program stores tasks in `tasks.txt`. This file is created automatically after you add or save tasks.

## Requirements

- Windows
- GCC compiler, such as MinGW-w64
- `make`, such as MinGW Make

## Compile

Open a terminal in this project folder and run:

```sh
make
```

This creates:

```text
study_planner.exe
```

If `make` is not installed, you can compile directly with:

```sh
gcc -Wall -Wextra -std=c11 src/main.c -o study_planner.exe
```

## Run

```sh
./study_planner.exe
```

On Windows Command Prompt, you can also run:

```bat
study_planner.exe
```

## Clean Build Files

```sh
make clean
```

This removes `study_planner.exe` and `tasks.txt`.
