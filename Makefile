CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = study_planner.exe
SRC = src/main.c src/task.c src/storage.c src/ui.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	-del /Q $(TARGET) 2>NUL
