# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g  # Добавлен -g для отладочной информации
LDFLAGS = -Llib
LDLIBS = -lgraph -lqueue

# Директории
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Исходные файлы
GRAPH_SRCS = $(SRC_DIR)/graph_func.c
IO_SRCS = $(SRC_DIR)/io.c
MENU_SRCS = $(SRC_DIR)/menu.c
QUEUE_SRCS = $(SRC_DIR)/queue.c
UI_SRCS = $(SRC_DIR)/ui.c
MAIN_SRC = $(SRC_DIR)/lab5/main.c

# Объектные файлы
GRAPH_OBJ = $(OBJ_DIR)/graph_func.o
IO_OBJ = $(OBJ_DIR)/io.o
MENU_OBJ = $(OBJ_DIR)/menu.o
QUEUE_OBJ = $(OBJ_DIR)/queue.o
UI_OBJ = $(OBJ_DIR)/ui.o
MAIN_OBJ = $(OBJ_DIR)/main.o

# Библиотеки
GRAPH_LIB = $(LIB_DIR)/libgraph.a
QUEUE_LIB = $(LIB_DIR)/libqueue.a

# Исполняемый файл
TARGET = $(BIN_DIR)/lab5

# Правила по умолчанию
all: directories $(TARGET)

# Создание необходимых директорий
directories:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

# Сборка исполняемого файла
$(TARGET): $(MAIN_OBJ) $(GRAPH_LIB) $(QUEUE_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Сборка библиотеки graph
$(GRAPH_LIB): $(GRAPH_OBJ) $(IO_OBJ) $(MENU_OBJ) $(UI_OBJ)
	ar rcs $@ $^

# Сборка библиотеки queue
$(QUEUE_LIB): $(QUEUE_OBJ)
	ar rcs $@ $^

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/lab5/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Запуск с Valgrind
valgrind: all
	valgrind --leak-check=full $(TARGET)

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

.PHONY: all directories clean valgrind