CC = gcc
CFLAGS = -Wall -Wextra -g -I../1
LDFLAGS = -ll

PARSER_DIR = ../1
PARSER_OBJS = $(PARSER_DIR)/parser.tab.o $(PARSER_DIR)/lex.yy.o $(PARSER_DIR)/ast.o

SRCS = main.c cfg.c cfg_builder.c cfg_dot.c
OBJS = $(SRCS:.c=.o)

TARGET = cfg_builder
PRINT_AST = print_ast

all: $(TARGET) $(PRINT_AST)

$(TARGET): $(OBJS) $(PARSER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PRINT_AST): test/print_ast.c $(PARSER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PARSER_DIR)/parser.tab.c $(PARSER_DIR)/parser.tab.h: $(PARSER_DIR)/parser.y
	cd $(PARSER_DIR) && bison -d parser.y

$(PARSER_DIR)/lex.yy.c: $(PARSER_DIR)/lexer.l $(PARSER_DIR)/parser.tab.h
	cd $(PARSER_DIR) && flex lexer.l

$(PARSER_DIR)/%.o: $(PARSER_DIR)/%.c
	cd $(PARSER_DIR) && $(CC) $(CFLAGS) -c $(notdir $<)

%.o: %.c cfg.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) $(PRINT_AST)
	rm -f *.dot *.png

test: $(TARGET)
	./$(TARGET) test1.src output
	@echo "\nGenerating images..."
	@for f in output/*.dot; do \
		if [ -f "$$f" ]; then \
			dot -Tpng "$$f" -o "$${f%.dot}.png"; \
			echo "Generated $${f%.dot}.png"; \
		fi \
	done

.PHONY: all clean test
