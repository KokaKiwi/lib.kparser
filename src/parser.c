#include <stdlib.h>
#include "klp.h"
#include "klp_stack.h"

void klp_parser_init(klp_parser_t *parser, void *data)
{
    klp_stack_init(&(parser->stack), -1);
    klp_stack_push(&(parser->stack), T_SYM_EOS);
    klp_stack_push(&(parser->stack), NT_SYM_START);
    parser->data = data;
}

int klp_parser_parse(klp_parser_t *parser)
{
    rule_handler_t rule_handler;

    parser->current_symbol = parser->lexer(parser->data);

    while (parser->stack.next != &(parser->stack))
    {
        if (parser->current_symbol == klp_stack_peek(&(parser->stack)))
        {
            parser->current_symbol = parser->lexer(parser->data);
            klp_stack_pop(&(parser->stack));
        }
        else
        {
            rule_handler = parser->get_rule_handler(parser, klp_stack_peek(&(parser->stack)), parser->current_symbol);
            if (rule_handler != NULL)
                rule_handler(&(parser->stack), parser->data);
            else
                return 1;
        }
    }
    return 0;
}
