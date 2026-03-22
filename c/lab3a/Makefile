# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall

# Библиотеки
LIBS = -lreadline

# Директории
SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib
BIN_DIR = bin

# Исходные файлы для библиотеки
LIB_SRC_FILES = $(wildcard $(SRC_DIR)/table_func.c $(SRC_DIR)/io.c)
LIB_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(LIB_SRC_FILES))
LIB_NAME = libtable.a
LIB_TARGET = $(LIB_DIR)/$(LIB_NAME)

# Исходные файлы для прикладной программы
APP_SRC_FILES = $(wildcard $(SRC_DIR)/main.c $(SRC_DIR)/menu.c)
APP_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(APP_SRC_FILES))
APP_NAME = lab3a
APP_TARGET = $(BIN_DIR)/$(APP_NAME)

# Правило по умолчанию
all: lib app

# Сборка библиотеки
lib: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJ_FILES)
	@mkdir -p $(LIB_DIR)
	ar rcs $(LIB_TARGET) $(LIB_OBJ_FILES)

# Сборка прикладной программы
app: $(APP_TARGET)

$(APP_TARGET): $(APP_OBJ_FILES) $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(APP_OBJ_FILES) -o $(APP_TARGET) -L$(LIB_DIR) -ltable $(LIBS)

# Правило для компиляции объектных файлов в bin
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(LIB_DIR) $(BIN_DIR)

# Запуск с Valgrind
valgrind: app
	valgrind --leak-check=full $(APP_TARGET)

# Флаг для предотвращения конфликтов
.PHONY: all lib app clean valgrind