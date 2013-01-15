#ifndef KLP_H_
#define KLP_H_

#include "klp_stack.h"

#define NT_SYM_START        0

#define T_SYM_EOS           0
#define T_SYM_UNDEFINED     1

typedef struct __klp_parser_t klp_parser_t;
typedef void (*rule_handler_t)(klp_stack_t *, void *);

struct __klp_parser_t
{
    klp_stack_t stack;
    int current_symbol;
    int (*lexer)(void *);
    rule_handler_t (*get_rule_handler)(klp_parser_t *, int, int);
    void *data;
};

void klp_parser_init(klp_parser_t *, void *);
int klp_parser_parse(klp_parser_t *);

#endif /* KLP_H_ */
