# Компилятор и флаги
CC = gcc
CFLAGS = -I$(INCLUDE_DIR)
LIBS = -lreadline

# Директории
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj

# Файлы библиотеки
LIB_SRC = $(SRC_DIR)/table_func.c $(SRC_DIR)/io.c
LIB_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIB_SRC))
LIB_TARGET = $(LIB_DIR)/libtable.a

# Файлы приложения
APP_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c
APP_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SRC))
APP_TARGET = $(BIN_DIR)/lab3a

all: lib app

# Сборка библиотеки
lib: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJ)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Сборка приложения
app: $(APP_TARGET)

$(APP_TARGET): $(APP_OBJ) | $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) -ltable $(LIBS)

# Общее правило компиляции .c → .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Запуск с Valgrind
valgrind: app
	valgrind --leak-check=full $(APP_TARGET)

# Очистка
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR)

# Флаг для предотвращения конфликтов
.PHONY: all lib app clean valgrind