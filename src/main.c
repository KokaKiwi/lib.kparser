#include <stdlib.h>
#include <stdio.h>
#include "klp.h"

#define T_SYM_PLUS 256
#define T_SYM_RPAREN 257
#define T_SYM_LPAREN 258
#define T_SYM_A 259

#define NT_SYM_START_FOLLOW 256
#define NT_SYM_DATA 257
#define NT_SYM_EXPR 258

void rule_START_FOLLOW(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, T_SYM_RPAREN);
    klp_stack_push(stack, NT_SYM_DATA);
    klp_stack_push(stack, T_SYM_PLUS);
}

void rule_START_FOLLOW_1(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, T_SYM_RPAREN);
}

void rule_START(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, NT_SYM_EXPR);
}

void rule_START_1(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, NT_SYM_START_FOLLOW);
    klp_stack_push(stack, NT_SYM_START);
    klp_stack_push(stack, T_SYM_LPAREN);
}

void rule_DATA(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, NT_SYM_EXPR);
}

void rule_DATA_1(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, T_SYM_RPAREN);
    klp_stack_push(stack, NT_SYM_EXPR);
    klp_stack_push(stack, T_SYM_LPAREN);
}

void rule_EXPR(klp_stack_t *stack, void *data)
{
    klp_stack_pop(stack);
    klp_stack_push(stack, T_SYM_A);
}

rule_handler_t test_get_rule_handler(klp_parser_t *parser, int ntsym, int tsym)
{
    if (ntsym == NT_SYM_START_FOLLOW && tsym == T_SYM_RPAREN)
        return &rule_START_FOLLOW_1;
    else if (ntsym == NT_SYM_START_FOLLOW && tsym == T_SYM_PLUS)
        return &rule_START_FOLLOW;
    else if (ntsym == NT_SYM_START && tsym == T_SYM_A)
        return &rule_START;
    else if (ntsym == NT_SYM_START && tsym == T_SYM_LPAREN)
        return &rule_START_1;
    else if (ntsym == NT_SYM_DATA && tsym == T_SYM_A)
        return &rule_DATA;
    else if (ntsym == NT_SYM_DATA && tsym == T_SYM_LPAREN)
        return &rule_DATA_1;
    else if (ntsym == NT_SYM_EXPR && tsym == T_SYM_A)
        return &rule_EXPR;
    return NULL;
}

struct
{
    char *input;
    int pos;
}   reader = {NULL, 0};

static int test_lex(void *data)
{
    char c = reader.input[reader.pos++];

    switch (c)
    {
        case '(':
            return T_SYM_LPAREN;
            break;

        case ')':
            return T_SYM_RPAREN;
            break;

        case 'a':
            return T_SYM_A;
            break;

        case '+':
            return T_SYM_PLUS;
            break;

        case 0:
            return T_SYM_EOS;
            break;
    }
    return T_SYM_UNDEFINED;
}

int main(int argc, char **argv)
{
    klp_parser_t parser;

    if (argc == 2)
    {
        reader.input = argv[1];
        klp_parser_init(&parser, NULL);
        parser.lexer = &test_lex;
        parser.get_rule_handler = &test_get_rule_handler;
        printf("%d\n", klp_parser_parse(&parser));
    }

    return EXIT_SUCCESS;
}
