#ifndef KLP_STACK_H_
#define KLP_STACK_H_

typedef struct __klp_stack_t klp_stack_t;
struct __klp_stack_t
{
    int symbol;
    klp_stack_t *prev, *next;
};

klp_stack_t *klp_stack_create(int);
void klp_stack_init(klp_stack_t *, int);

void klp_stack_add(klp_stack_t *, klp_stack_t *);

void klp_stack_remove(klp_stack_t *);
void klp_stack_delete(klp_stack_t *);
void klp_stack_destroy(klp_stack_t *);

void klp_stack_push(klp_stack_t *, int);
void klp_stack_pushleft(klp_stack_t *, int);

int klp_stack_pop(klp_stack_t *);
int klp_stack_popleft(klp_stack_t *);

int klp_stack_peek(klp_stack_t *);

int klp_stack_size(klp_stack_t *);

#endif /* KLP_STACK_H_ */
