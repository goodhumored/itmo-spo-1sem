CC = gcc
CFLAGS = -Wall -Wextra -g -I../1 -I../2
LDFLAGS = -ll

# Директории
SRC_DIR = src
CODEGEN_DIR = $(SRC_DIR)/codegen
BUILD_DIR = build
BUILD_CODEGEN_DIR = $(BUILD_DIR)/codegen

# Файлы из предыдущих заданий
PARSER_DIR = ../1
CFG_DIR = ../2

# Объектные файлы парсера
PARSER_OBJS = $(PARSER_DIR)/parser.tab.o $(PARSER_DIR)/lex.yy.o $(PARSER_DIR)/ast.o

# Объектные файлы CFG
CFG_OBJS = $(CFG_DIR)/cfg.o $(CFG_DIR)/cfg_builder.o $(CFG_DIR)/cfg_dot.o

# Исходные файлы проекта
MAIN_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/asm_types.c $(SRC_DIR)/utils.c
CODEGEN_SRCS = $(CODEGEN_DIR)/register_allocator.c $(CODEGEN_DIR)/function_context.c \
               $(CODEGEN_DIR)/function_gen.c $(CODEGEN_DIR)/operand_utils.c \
               $(CODEGEN_DIR)/operation_gen.c $(CODEGEN_DIR)/block_gen.c \
	       $(CODEGEN_DIR)/string_utils.c

MAIN_OBJS = $(MAIN_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
CODEGEN_OBJS = $(CODEGEN_SRCS:$(CODEGEN_DIR)/%.c=$(BUILD_CODEGEN_DIR)/%.o)
OBJS = $(MAIN_OBJS) $(CODEGEN_OBJS)

# Цели
TARGET = codegen
TARGET_DIR = .

# Правила по умолчанию
all: $(TARGET)

# Создание директорий для сборки
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_CODEGEN_DIR):
	mkdir -p $(BUILD_CODEGEN_DIR)

# Компиляция исходных файлов проекта
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция модулей codegen
$(BUILD_CODEGEN_DIR)/%.o: $(CODEGEN_DIR)/%.c | $(BUILD_DIR)
	mkdir -p $(BUILD_CODEGEN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Генерация файлов парсера (если необходимо)
$(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/parser.tab.h:
	cd $(PARSER_DIR) && bison -d parser.y

$(PARSER_DIR)/lex.yy.c: $(PARSER_DIR)/lexer.l $(PARSER_DIR)/parser.tab.h
	cd $(PARSER_DIR) && flex lexer.l

# Компиляция файлов парсера
$(PARSER_DIR)/%.o: $(PARSER_DIR)/%.c
	cd $(PARSER_DIR) && $(CC) $(CFLAGS) -c $(notdir $<) -o $(notdir $@)

# Компиляция файлов CFG
$(CFG_DIR)/%.o: $(CFG_DIR)/%.c
	cd $(CFG_DIR) && $(CC) $(CFLAGS) -c $(notdir $<) -o $(notdir $@)

# Сборка основного приложения
$(TARGET): $(BUILD_DIR) $(BUILD_CODEGEN_DIR) $(OBJS) $(PARSER_OBJS) $(CFG_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_DIR)/$@ $(OBJS) $(PARSER_OBJS) $(CFG_OBJS) $(LDFLAGS)

# Запуск теста
test: $(TARGET)
	./$(TARGET) ../2/test1.src output
	@echo "\\nGenerated files in output directory:"
	@ls -la output/

# Очистка
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_DIR)/$(TARGET)
	rm -f output/*

# Правила для фальшивых целей
.PHONY: all clean test
