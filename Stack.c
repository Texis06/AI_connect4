#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct SNode
{
    uint64_t value;
    struct SNode* prev;
}SNode;

typedef struct
{
    SNode* top;
    int size;
}Stack;

void initialize(Stack *s)
{
    s->size=0;
    s->top=NULL;
}

bool isEmpty(Stack *s)
{
    return s->size==0;
}

void add(Stack *s, uint64_t value)
{
    s->size++;
    SNode* newNode = malloc(sizeof(SNode));
    newNode -> value = value;
    newNode -> prev = s -> top;
    s -> top = newNode;
}

uint64_t sPop(Stack *s)
{
    if(s->size==0) return 0;
    s->size--;
    SNode* oldNode = s->top;
    uint64_t value = oldNode -> value;
    s -> top = oldNode -> prev;
    free(oldNode);
    return value;
}

void clear(Stack *s)
{
    while(sPop(s));
}

void printStack(Stack *s)
{
    if(s -> size == 0) {return; }
    SNode* node = s -> top;
    printf("\n%llu,", node -> value);
    while(node -> prev != NULL)
    {
        node = node -> prev;
        printf("%llu", node -> value);
        if(node -> prev != NULL) {printf(","); }
    }
}
