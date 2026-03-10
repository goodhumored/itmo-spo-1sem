#include "cfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct BuildContext {
    CFG *cfg;
    const char *function_name;  // имя функции для генерации полных меток
    BasicBlock *current_block;
    BasicBlock *break_target;
    BasicBlock *continue_target;
    int next_temp_id;  // для генерации t0, t1, t2, ...
    OperationType last_cmp_type;  // для хранения типа последнего сравнения (OP_LT, OP_GT и т.д.)
} BuildContext;

static int get_next_temp(BuildContext *ctx) {
    return ctx->next_temp_id++;
}

static void emit_op(BuildContext *ctx, Operation *op) {
    if (!ctx->current_block) {
        fprintf(stderr, "Error: No current block for operation\n");
        free_operation(op);
        return;
    }
    add_operation_to_block(ctx->current_block, op);
}

static Operation* create_const_op(int64_t val) {
    Operand const_op = create_const_operand(val);
    Operand dest = create_temp_operand(-1);  // temp id будет назначен вызывателем
    return create_operation(OP_CONST, const_op, create_undef_operand(), create_undef_operand());
}

static Operation* create_binary_op(const char *op_str, Operand left, Operand right, Operand dest) {
    OperationType type;

    if (strcmp(op_str, "+") == 0) type = OP_ADD;
    else if (strcmp(op_str, "-") == 0) type = OP_SUB;
    else if (strcmp(op_str, "*") == 0) type = OP_MUL;
    else if (strcmp(op_str, "/") == 0) type = OP_DIV;
    else if (strcmp(op_str, "%") == 0) type = OP_MOD;
    else if (strcmp(op_str, "<") == 0) type = OP_LT;
    else if (strcmp(op_str, ">") == 0) type = OP_GT;
    else if (strcmp(op_str, "&") == 0 || strcmp(op_str, "|") == 0 || strcmp(op_str, "^") == 0) {
        fprintf(stderr, "Warning: Bitwise operations not yet implemented\n");
        type = OP_NOP;
    }
    else {
        fprintf(stderr, "Error: Unknown binary operator: %s\n", op_str);
        type = OP_NOP;
    }

    return create_operation(type, left, right, dest);
}

static Operation* create_store_op(Operand var, Operand value) {
    return create_operation(OP_STORE, value, var, create_undef_operand());
}

static Operation* create_load_op(Operand var, Operand dest) {
    return create_operation(OP_LOAD, var, create_undef_operand(), dest);
}

static Operation* create_call_op(const char *func_name, Operand dest, ASTNode *ast_node) {
    Operand func_label = create_label_operand(func_name);
    Operation *op = create_operation(OP_CALL, func_label, create_undef_operand(), dest);
    op->ast_node = ast_node;  // Сохраняем AST для доступа к аргументам
    return op;
}

static Operation* create_jmp_op(const char *target) {
    Operand target_label = create_label_operand(target);
    return create_operation(OP_JMP, target_label, create_undef_operand(), create_undef_operand());
}

static void process_statement(BuildContext *ctx, ASTNode *stmt);
static void process_statement_list(BuildContext *ctx, ASTNode *stmt_list);

static const char* get_node_name(ASTNode *node) {
    if (!node) return NULL;
    if (node->value) return node->value;
    return node->type;
}

static ASTNode* find_child_by_type(ASTNode *node, const char *type) {
    if (!node) return NULL;
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i] && strcmp(node->children[i]->type, type) == 0) {
            return node->children[i];
        }
    }
    return NULL;
}

static Operand process_expression(BuildContext *ctx, ASTNode *expr) {
    if (!expr || !ctx) return create_undef_operand();

    const char *type = expr->type;

    // литералы
    if (strcmp(type, "literal") == 0 && expr->value) {
        int64_t val;
        if (sscanf(expr->value, "%lld", (long long*)&val) == 1 ||
            strcmp(expr->value, "true") == 0 || strcmp(expr->value, "false") == 0) {
            if (strcmp(expr->value, "true") == 0) val = 1;
            else if (strcmp(expr->value, "false") == 0) val = 0;
            return create_const_operand(val);
        }
        // Для строковых литералов пока вернём как const 0 (можно расширить)
        // TODO: Добавить поддержку строковых литералов
        return create_const_operand(0);
    }

    // Identifier
    if (strcmp(type, "identifier") == 0 && expr->value) {
        return create_var_operand(expr->value);
    }

    // Бинарные операции
    if (strcmp(type, AST_BINARY) == 0) {
        const char *op = expr->value ? expr->value : "?";

        if (strcmp(op, "=") == 0 && expr->child_count >= 2) {
            // Assignment: x = value
            Operand left = process_expression(ctx, expr->children[0]);
            Operand right = process_expression(ctx, expr->children[1]);
            Operation *store_op = create_store_op(left, right);
            emit_op(ctx, store_op);
            return left;  // Assignment возвращает left operand
        } else {
            // Обычная бинарная операция (арифметика или сравнение)
            Operand left = process_expression(ctx, expr->children[0]);
            Operand right = process_expression(ctx, expr->children[1]);
            Operand dest = create_temp_operand(get_next_temp(ctx));
            Operation *bin_op = create_binary_op(op, left, right, dest);
            
            // Сохраняем тип операции если это сравнение
            if (bin_op->type == OP_LT || bin_op->type == OP_GT) {
                ctx->last_cmp_type = bin_op->type;
            }
            
            emit_op(ctx, bin_op);
            return dest;
        }
    }

    // Унарные операции
    if (strcmp(type, AST_UNARY) == 0) {
        const char *op = expr->value ? expr->value : "?";
        Operand operand = process_expression(ctx, expr->children[0]);
        Operand dest = create_temp_operand(get_next_temp(ctx));

        // TODO: Добавить поддержку унарных операций
        fprintf(stderr, "Warning: Unary operation '%s' not yet implemented\n", op);
        return operand;
    }

    // Вызовы функций
    if (strcmp(type, AST_CALL) == 0) {
        const char *func_name = "unknown";
        if (expr->child_count > 0 && expr->children[0] && expr->children[0]->value) {
            func_name = expr->children[0]->value;
        }
        Operand dest = create_temp_operand(get_next_temp(ctx));
        Operation *call_op = create_call_op(func_name, dest, expr);
        emit_op(ctx, call_op);
        return dest;
    }

    return create_undef_operand();
}

static void extract_function_info(CFG *cfg, ASTNode *func_node) {
    ASTNode *sig = find_child_by_type(func_node, "funcSignature");
    if (!sig) return;
    
    if (sig->value) {
        free(cfg->function->name);
        cfg->function->name = strdup(sig->value);
    }
    
    for (int i = 0; i < sig->child_count; i++) {
        ASTNode *child = sig->children[i];
        if (!child) continue;
        
        if (strcmp(child->type, "argDefList") == 0) {
            for (int j = 0; j < child->child_count; j++) {
                ASTNode *arg_def = child->children[j];
                if (arg_def && strcmp(arg_def->type, "argDef") == 0) {
                    const char *arg_name = arg_def->value ? arg_def->value : "arg";
                    const char *arg_type = NULL;
                    
                    if (arg_def->child_count > 0 && arg_def->children[0]) {
                        ASTNode *type_node = arg_def->children[0];
                        arg_type = get_node_name(type_node);
                    }
                    
                    add_argument(cfg->function, arg_name, arg_type);
                }
            }
        } else if (child->value || strcmp(child->type, "builtinType") == 0 ||
                   strcmp(child->type, "customType") == 0 ||
                   strcmp(child->type, "arrayType") == 0) {
            if (!cfg->function->return_type) {
                const char *type_name = get_node_name(child);
                if (type_name) {
                    cfg->function->return_type = strdup(type_name);
                }
            }
        }
    }
}

static void extract_local_vars(CFG *cfg, ASTNode *block) {
    if (!block || strcmp(block->type, "block") != 0) return;
    
    for (int i = 0; i < block->child_count; i++) {
        ASTNode *stmt = block->children[i];
        if (!stmt) continue;
        
        if (strcmp(stmt->type, "varDecl") == 0) {
            const char *var_type = NULL;
            
            if (stmt->child_count > 0 && stmt->children[0]) {
                var_type = get_node_name(stmt->children[0]);
            }
            
            ASTNode *var_list = find_child_by_type(stmt, "varList");
            if (var_list) {
                for (int j = 0; j < var_list->child_count; j++) {
                    ASTNode *var_item = var_list->children[j];
                    if (var_item && var_item->value) {
                        add_local_var(cfg->function, var_item->value, var_type);
                    }
                }
            }
        }
    }
}

static void process_if_statement(BuildContext *ctx, ASTNode *if_stmt) {
    ASTNode *condition = if_stmt->child_count > 0 ? if_stmt->children[0] : NULL;
    ASTNode *then_stmt = if_stmt->child_count > 1 ? if_stmt->children[1] : NULL;
    ASTNode *else_stmt = if_stmt->child_count > 2 ? if_stmt->children[2] : NULL;

    // Генерируем условие и получаем результат в Operand
    Operand cond_result = process_expression(ctx, condition);

    // Создаём блоки для then, else и merge
    BasicBlock *then_block = create_basic_block(ctx->cfg, "then");
    BasicBlock *merge_block = create_basic_block(ctx->cfg, "merge");
    add_block_to_cfg(ctx->cfg, then_block);
    add_block_to_cfg(ctx->cfg, merge_block);

    BasicBlock *else_block = NULL;
    if (else_stmt && strcmp(else_stmt->type, AST_IF) != 0) {
        // Обычный else (не else if)
        else_block = create_basic_block(ctx->cfg, "else");
        add_block_to_cfg(ctx->cfg, else_block);
    }

    // Создаём conditional jump в текущем блоке
    // Генерируем полные метки для jmp
    char then_full_label[128];
    char false_full_label[128];
    snprintf(then_full_label, sizeof(then_full_label), "%s_%s", 
             ctx->function_name, then_block->label);

    if (else_block) {
        snprintf(false_full_label, sizeof(false_full_label), "%s_%s",
                 ctx->function_name, else_block->label);
    } else {
        snprintf(false_full_label, sizeof(false_full_label), "%s_%s",
                 ctx->function_name, merge_block->label);
    }

    // Создаём операцию conditional jump
    Operation *cjmp_op = create_operation(OP_CJMP,
                                         cond_result,
                                         create_undef_operand(),
                                         create_undef_operand());
    cjmp_op->cmp_type = ctx->last_cmp_type;
    cjmp_op->true_target = strdup(then_full_label);
    cjmp_op->false_target = strdup(false_full_label);
    emit_op(ctx, cjmp_op);

    // Связываем successors
    add_successor(ctx->current_block, then_block);
    if (else_block) {
        add_successor(ctx->current_block, else_block);
        add_successor(ctx->current_block, merge_block);
    } else {
        add_successor(ctx->current_block, merge_block);
    }

    // Обрабатываем then ветку
    ctx->current_block = then_block;
    if (then_stmt) {
        process_statement(ctx, then_stmt);
    }
    // Добавляем безусловный переход к merge если есть else или если не все пути заканчиваются
    if (else_block || (ctx->current_block && ctx->current_block != merge_block)) {
        add_successor(ctx->current_block, merge_block);
    }

    // Обрабатываем else ветку
    if (else_stmt) {
        if (strcmp(else_stmt->type, AST_IF) == 0) {
            // else if - рекурсивно обрабатываем
            ctx->current_block = merge_block;
            process_if_statement(ctx, else_stmt);
        } else {
            // Обычный else
            ctx->current_block = else_block;
            process_statement(ctx, else_stmt);
            add_successor(ctx->current_block, merge_block);
        }
    }

    ctx->current_block = merge_block;
}

static void process_while_statement(BuildContext *ctx, ASTNode *while_stmt) {
    ASTNode *condition = while_stmt->child_count > 0 ? while_stmt->children[0] : NULL;
    ASTNode *body = while_stmt->child_count > 1 ? while_stmt->children[1] : NULL;
    
    // Создаём блоки для while
    BasicBlock *loop_header = create_basic_block(ctx->cfg, "while_cond");
    BasicBlock *loop_body = create_basic_block(ctx->cfg, "while_body");
    BasicBlock *loop_exit = create_basic_block(ctx->cfg, "while_exit");
    
    add_block_to_cfg(ctx->cfg, loop_header);
    add_block_to_cfg(ctx->cfg, loop_body);
    add_block_to_cfg(ctx->cfg, loop_exit);
    
    // Связываем вход с header
    add_successor(ctx->current_block, loop_header);
    
    // Обрабатываем условие в header
    ctx->current_block = loop_header;
    if (condition) {

    Operand cond_result = process_expression(ctx, condition);

    char body_full_label[128];
    char exit_full_label[128];
    snprintf(body_full_label, sizeof(body_full_label), "%s_%s",
             ctx->function_name, loop_body->label);
    snprintf(exit_full_label, sizeof(exit_full_label), "%s_%s",
             ctx->function_name, loop_exit->label);

    Operation *cjmp_op = create_operation(OP_CJMP,
                                         cond_result,
                                         create_undef_operand(),
                                         create_undef_operand());
    cjmp_op->true_target = strdup(body_full_label);
    cjmp_op->false_target = strdup(exit_full_label);
    cjmp_op->cmp_type = ctx->last_cmp_type;
    emit_op(ctx, cjmp_op);

    add_successor(loop_header, loop_body);
    add_successor(loop_header, loop_exit);
    }
    
    // Обрабатываем тело цикла
    BasicBlock *saved_break = ctx->break_target;
    BasicBlock *saved_continue = ctx->continue_target;
    ctx->break_target = loop_exit;
    ctx->continue_target = loop_header;
    
    if (body) {
        ctx->current_block = loop_body;
        process_statement(ctx, body);
    }

    if (ctx->current_block) {
        char header_full_label[128];
        snprintf(header_full_label, sizeof(header_full_label), "%s_%s",
                 ctx->function_name, loop_header->label);
        Operation *jmp_op = create_jmp_op(header_full_label);
        emit_op(ctx, jmp_op);
        add_successor(ctx->current_block, loop_header);
    }
    
    ctx->break_target = saved_break;
    ctx->continue_target = saved_continue;
    ctx->current_block = loop_exit;
}

static void process_do_while_statement(BuildContext *ctx, ASTNode *do_stmt) {
    ASTNode *body = do_stmt->child_count > 0 ? do_stmt->children[0] : NULL;
    ASTNode *condition = do_stmt->child_count > 1 ? do_stmt->children[1] : NULL;

    BasicBlock *loop_body = create_basic_block(ctx->cfg, "do_body");
    BasicBlock *loop_cond = create_basic_block(ctx->cfg, "do_cond");
    BasicBlock *loop_exit = create_basic_block(ctx->cfg, "do_exit");

    add_block_to_cfg(ctx->cfg, loop_body);
    add_block_to_cfg(ctx->cfg, loop_cond);
    add_block_to_cfg(ctx->cfg, loop_exit);

    add_successor(ctx->current_block, loop_body);

    BasicBlock *saved_break = ctx->break_target;
    BasicBlock *saved_continue = ctx->continue_target;
    ctx->break_target = loop_exit;
    ctx->continue_target = loop_cond;

    ctx->current_block = loop_body;
    if (body) {
        process_statement(ctx, body);
    }

    if (ctx->current_block) {
        add_successor(ctx->current_block, loop_cond);
    }

    ctx->current_block = loop_cond;
    Operand cond_result = process_expression(ctx, condition);

    char body_full_label[128];
    char exit_full_label[128];
    snprintf(body_full_label, sizeof(body_full_label), "%s_%s",
             ctx->function_name, loop_body->label);
    snprintf(exit_full_label, sizeof(exit_full_label), "%s_%s",
             ctx->function_name, loop_exit->label);

    Operation *cjmp_op = create_operation(OP_CJMP,
                                         cond_result,
                                         create_undef_operand(),
                                         create_undef_operand());
    cjmp_op->true_target = strdup(body_full_label);
    cjmp_op->cmp_type = ctx->last_cmp_type;
    cjmp_op->false_target = strdup(exit_full_label);
    emit_op(ctx, cjmp_op);

    add_successor(loop_cond, loop_body);
    add_successor(loop_cond, loop_exit);

    ctx->break_target = saved_break;
    ctx->continue_target = saved_continue;
    ctx->current_block = loop_exit;
}

static void process_break_statement(BuildContext *ctx, ASTNode *break_stmt) {
    if (ctx->break_target) {
        add_successor(ctx->current_block, ctx->break_target);
        ctx->current_block = NULL;
    }
}

static void process_statement(BuildContext *ctx, ASTNode *stmt) {
    if (!stmt || !ctx->current_block) return;

    if (strcmp(stmt->type, AST_IF) == 0) {
        process_if_statement(ctx, stmt);
    } else if (strcmp(stmt->type, AST_WHILE) == 0) {
        process_while_statement(ctx, stmt);
    } else if (strcmp(stmt->type, AST_DO) == 0) {
        process_do_while_statement(ctx, stmt);
    } else if (strcmp(stmt->type, AST_BREAK_STMT) == 0) {
        process_break_statement(ctx, stmt);
    } else if (strcmp(stmt->type, AST_BLOCK) == 0) {
        process_statement_list(ctx, stmt);
    } else if (strcmp(stmt->type, AST_EXPR_STMT) == 0) {
        if (stmt->child_count > 0) {
            Operand result = process_expression(ctx, stmt->children[0]);
            // Если результат не используется, просто игнорируем
        }
    } else if (strcmp(stmt->type, "varDecl") == 0) {
        // Объявление переменных: добавляем Operations для инициализации
        ASTNode *var_list = find_child_by_type(stmt, "varList");
        if (var_list) {
            for (int i = 0; i < var_list->child_count; i++) {
                ASTNode *var_item = var_list->children[i];
                if (var_item && strcmp(var_item->type, "varInit") == 0) {
                    // var with initialization: var = expr
                    if (var_item->value && var_item->child_count > 0) {
                        Operand var_dest = create_var_operand(var_item->value);
                        Operand init_val = process_expression(ctx, var_item->children[0]);
                        Operation *store_op = create_store_op(var_dest, init_val);
                        emit_op(ctx, store_op);
                    }
                }
                // var without initialization: можно вставить OP_CONST 0 или пропустить
                // TODO: решить нужно ли инициализировать нулём
            }
        }
    } else {
        // Для других типов statements пока просто игнорируем
        fprintf(stderr, "Warning: Unknown statement type: %s\n", stmt->type);
    }
}

static void process_statement_list(BuildContext *ctx, ASTNode *stmt_list) {
    if (!stmt_list) return;
    
    for (int i = 0; i < stmt_list->child_count; i++) {
        if (!ctx->current_block) {
            BasicBlock *new_block = create_basic_block(ctx->cfg, "unreachable");
            add_block_to_cfg(ctx->cfg, new_block);
            ctx->current_block = new_block;
        }
        process_statement(ctx, stmt_list->children[i]);
    }
}

CFG* build_cfg_from_function(ASTNode *func_node) {
    if (!func_node || strcmp(func_node->type, "funcDef") != 0) {
        return NULL;
    }
    
    CFG *cfg = create_cfg("unknown", NULL);
    
    extract_function_info(cfg, func_node);
    
    ASTNode *body = find_child_by_type(func_node, "block");
    if (!body) {
        cfg->function->entry_block = create_basic_block(cfg, "entry");
        cfg->function->exit_block = cfg->function->entry_block;
        add_block_to_cfg(cfg, cfg->function->entry_block);
        return cfg;
    }
    
    extract_local_vars(cfg, body);
    
    BasicBlock *entry = create_basic_block(cfg, "entry");
    BasicBlock *exit = create_basic_block(cfg, "exit");
    
    cfg->function->entry_block = entry;
    cfg->function->exit_block = exit;
    
    add_block_to_cfg(cfg, entry);
    add_block_to_cfg(cfg, exit);
    
    BuildContext ctx = {
        .cfg = cfg,
        .function_name = cfg->function->name,
        .current_block = entry,
        .break_target = NULL,
        .continue_target = NULL
    };
    
    process_statement_list(&ctx, body);
    
    if (ctx.current_block && ctx.current_block != exit) {
        add_successor(ctx.current_block, exit);
    }
    
    return cfg;
}
