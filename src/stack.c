#include <stdlib.h>
#include "klp_stack.h"

klp_stack_t *klp_stack_create(int symbol)
{
    klp_stack_t *node = malloc(sizeof(klp_stack_t));

    if (node != NULL)
        klp_stack_init(node, symbol);
    return node;
}

void klp_stack_init(klp_stack_t *node, int symbol)
{
    node->symbol = symbol;
    node->prev = node->next = node;
}

void klp_stack_add(klp_stack_t *prev, klp_stack_t *node)
{
    prev->next->prev = node;
    node->next = prev->next;
    prev->next = node;
    node->prev = prev;
}

void klp_stack_remove(klp_stack_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = node;
}

void klp_stack_delete(klp_stack_t *node)
{
    klp_stack_remove(node);
    free(node);
}

void klp_stack_destroy(klp_stack_t *stack)
{
    klp_stack_t *node = stack->next;

    while (node != stack)
    {
        node = node->next;
        klp_stack_delete(node->prev);
    }
}

void klp_stack_push(klp_stack_t *stack, int symbol)
{
    klp_stack_t *node = klp_stack_create(symbol);

    if (node != NULL)
        klp_stack_add(stack->prev, node);
}

void klp_stack_pushleft(klp_stack_t *stack, int symbol)
{
    klp_stack_t *node = klp_stack_create(symbol);

    if (node != NULL)
        klp_stack_add(stack, node);
}

int klp_stack_pop(klp_stack_t *stack)
{
    int symbol;

    symbol = -1;
    if (stack->prev != stack)
    {
        symbol = stack->prev->symbol;
        klp_stack_delete(stack->prev);
    }
    return symbol;
}

int klp_stack_popleft(klp_stack_t *stack)
{
    int symbol;

    symbol = -1;
    if (stack->next != stack)
    {
        symbol = stack->next->symbol;
        klp_stack_delete(stack->next);
    }
    return symbol;
}

int klp_stack_peek(klp_stack_t *stack)
{
    int symbol;

    symbol = -1;
    if (stack->prev != stack)
        symbol = stack->prev->symbol;
    return symbol;
}

int klp_stack_size(klp_stack_t *stack)
{
    klp_stack_t *node = stack->next;
    int size = 0;

    while (node != stack)
    {
        node = node->next;
        size++;
    }
    return size;
}
