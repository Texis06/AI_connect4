/*
#include "popout.c"

typedef struct {
  int state[MAX_X][MAX_Y];
  int totalvictory = 0;
  int totalVisits = 0;
  // número máximo de moves possíveis numa jogada é 14, logo 32 entries é o suficiente
  MCTSNode * nextNodes[32];
  MCTSNode * parentNode;
} MCTSNode;

// função para copiar um state para outra variável/board
void copy_board(char original[MAX_X][MAX_Y], char copy[MAX_X][MAX_Y]){
  for(int x=0;x<MAX_X;x++) {
    for(int y=0;y<MAX_Y;y++) {dst[x][y] = src[x][y];}
  }
}

*/

#include <math.h>
#include <time.h>
#include "popout.c"

/*=========================================================
MCTS STRUCTURES
=========================================================*/

typedef struct
{
    int pos;
    int option;
    /*
    option:
    0 = insert
    1 = pop
    */
} Move;


typedef struct Node
{
    char board[MAX_X][MAX_Y];

    char current_player;

    Move move;

    struct Node *parent;

    struct Node *children[32];

    int child_count;

    int visits;

    double wins;

    bool fully_expanded;

} Node;

void copy_board(char src[MAX_X][MAX_Y],
                char dst[MAX_X][MAX_Y])
{
    for(int x=0;x<MAX_X;x++)
    {
        for(int y=0;y<MAX_Y;y++)
        {
            dst[x][y] = src[x][y];
        }
    }
}


void load_theory_board(char src[MAX_X][MAX_Y])
{
    for(int x=0;x<MAX_X;x++)
    {
        for(int y=0;y<MAX_Y;y++)
        {
            theory_board[x][y] = src[x][y];
        }
    }
}


void save_theory_board(char dst[MAX_X][MAX_Y])
{
    for(int x=0;x<MAX_X;x++)
    {
        for(int y=0;y<MAX_Y;y++)
        {
            dst[x][y] = theory_board[x][y];
        }
    }
}

void clear_ai_options()
{
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<2;j++)
        {
            ai_options[i][j] = false;
        }
    }
}

int generate_moves(Move moves[])
{
    clear_ai_options();

    ai_option_shower(false);

    int count = 0;

    for(int pos=1; pos<=7; pos++)
    {
        if(ai_options[pos][0])
        {
            moves[count].pos = pos;
            moves[count].option = 0;
            count++;
        }

        if(ai_options[pos][1])
        {
            moves[count].pos = pos;
            moves[count].option = 1;
            count++;
        }
    }

    if(ai_options[0][0])
    {
        moves[count].pos = 0;
        moves[count].option = 0;
        count++;
    }

    return count;
}

int apply_move(Move m)
{
    if(m.pos == 0)
    {
        return 0;
    }

    int result;

    if(m.option == 0)
    {
        result = theory_board_insert(m.pos);
    }
    else
    {
        result = theory_board_pop(m.pos);
    }

    player_switch();

    return result;
}

Node *create_node(Node *parent,
                  Move move,
                  char board[MAX_X][MAX_Y],
                  char current_player)
{
    Node *n = malloc(sizeof(Node));

    copy_board(board, n->board);

    n->current_player = current_player;

    n->move = move;

    n->parent = parent;

    n->child_count = 0;

    n->visits = 0;

    n->wins = 0.0;

    n->fully_expanded = false;

    return n;
}

double uct(Node *parent, Node *child)
{
    if(child->visits == 0)
    {
        return 1e9;
    }

    double exploitation = child->wins
                        / child->visits;

    double exploration = sqrt(log(parent->visits)
                         / child->visits);

    return exploitation + 1.414 * exploration;
}

Node *select_node(Node *root)
{
    Node *current = root;

    while(current->child_count > 0)
    {
        double best_score = -1e9;

        Node *best_child = NULL;

        for(int i=0;i<current->child_count;i++)
        {
            Node *child = current->children[i];

            double score = uct(current, child);

            if(score > best_score)
            {
                best_score = score;
                best_child = child;
            }
        }

        current = best_child;
    }

    return current;
}

Node *expand(Node *node)
{
    load_theory_board(node->board);

    player = node->current_player;

    Move moves[32];

    int move_count = generate_moves(moves);

    for(int i=0;i<move_count;i++)
    {
        load_theory_board(node->board);

        player = node->current_player;

        Move m = moves[i];

        apply_move(m);

        Node *child = create_node(node,
                                  m,
                                  theory_board,
                                  player);

        node->children[node->child_count++] = child;
    }

    node->fully_expanded = true;

    if(node->child_count == 0)
    {
        return node;
    }

    return node->children[rand()%node->child_count];
}

double rollout(Node *node)
{
    load_theory_board(node->board);

    player = node->current_player;

    for(int depth=0; depth<200; depth++)
    {
        Move moves[32];

        int count = generate_moves(moves);

        if(count == 0)
        {
            return 0.5;
        }

        Move m = moves[rand()%count];

        int result = apply_move(m);

        if(result == 1)
        {
            return 1.0;
        }

        if(result == -1)
        {
            return 0.0;
        }
    }

    return 0.5;
}

void backpropagate(Node *node,
                   double result)
{
    Node *current = node;

    while(current != NULL)
    {
        current->visits++;

        current->wins += result;

        result = 1.0 - result;

        current = current->parent;
    }
}

void free_tree(Node *node)
{
    if(node == NULL)
    {
        return;
    }

    for(int i=0;i<node->child_count;i++)
    {
        free_tree(node->children[i]);
    }

    free(node);
}

Move monte_carlo_move(int iterations)
                             root_move,
                             board,
                             player);

    for(int i=0;i<iterations;i++)
    {
        Node *selected = select_node(root);

        Node *expanded;

        if(selected->visits == 0)
        {
            expanded = selected;
        }
        else
        {
            expanded = expand(selected);
        }

        double result = rollout(expanded);

        backpropagate(expanded, result);
    }

    Node *best = NULL;

    int best_visits = -1;

    for(int i=0;i<root->child_count;i++)
    {
        Node *child = root->children[i];

        if(child->visits > best_visits)
        {
            best_visits = child->visits;
            best = child;
        }
    }

    Move answer = best->move;

    free_tree(root);

    return answer;
}

void ai_turn(int iterations)
{
    Move best = monte_carlo_move(iterations);

    printf("AI chose: column %d | %s
",
           best.pos,
           best.option == 0 ? "insert" : "pop");

    game_option_selector(best.pos,
                         best.option);
}

