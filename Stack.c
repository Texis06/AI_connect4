#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node
{
    int value;
    struct Node* prev;
}Node;

typedef struct
{
    Node* top;
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

void add(Stack *s, int value)
{
    s->size++;
    Node* newNode = malloc(sizeof(Node));
    newNode -> value = value;
    newNode -> prev = s -> top;
    s -> top = newNode;
}

bool pop(Stack *s)
{
    if(s->size==0) return false;
    s->size--;
    Node* oldNode = s->top;
    s -> top = oldNode -> prev;
    free(oldNode);
    return true;
}

void clear(Stack *s)
{
    while(pop(s));
}

void printStack(Stack *s)
{
    if(s -> size == 0) {return; }
    Node* node = s -> top;
    printf("\n%d,", node -> value);
    while(node -> prev != NULL)
    {
        node = node -> prev;
        printf("%d", node -> value);
        if(node -> prev != NULL) {printf(","); }
    }
}
