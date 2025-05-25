# Компилятор и флаги
CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -Llib
LDLIBS = -ltree -lreadline

SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

# Исходные файлы
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

LIB_OBJS = $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

.PHONY: all clean lib app

all: lib app

# Сборка библиотеки
lib: $(LIB_DIR)/libtree.a

$(LIB_DIR)/libtree.a: $(LIB_OBJS)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Сборка приложения
app: $(BIN_DIR)/lab4b

$(BIN_DIR)/lab4b: $(OBJ_DIR)/main.o $(LIB_DIR)/libtree.a
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)