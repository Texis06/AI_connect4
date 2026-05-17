#include <math.h>
#include <time.h>
#include "popout.c"

char theory_player;

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

void theory_player_switch();

// copia um board state para outra board
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

// copia um board state para o theory board
// theory board é o board do "jogo falso" do MCTS
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

// copia o state atual do theory board para outro (efetivamente guardando-o)
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

// dá reset às opções de jogadas do AI (para usar no final de uma jogada)
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

// dá novas opções de jogaadas ao AI
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

// manda o AI fazer um movimento
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


    return result;
}

// função para criar um node com um estado da board num local específico da árvore do MCTS 
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

// cálculo do Upper Confidence Bound para árvore
// este valor é o que decide qual é o node mais "promissor" (ou seja, qual é que vai ser jogado)
double uct(Node *parent, Node *child)
{
    if(child->visits == 0)
    {
        return 1e9;
    }
     //exploitation é o rácio direto entre jogos vencidos e totais (também chamado normalmente de "win ratio")
    double exploitation = child->wins
                        / child->visits;
     // exploration dá prioridade aos nodes que foram escolhidos para uma simulação mais vezes
     /* o parent, devido a backpropatation, vai ter todas as visitas feitas aos seus filhos como as suas visitas
      * logo, a divisão dentro da raiz quadrada irá dar um rácio que demonstra qual a parte de visitas aos filhos que foi este filho em específico
      */
    double exploration = sqrt(log(parent->visits)
                         / child->visits);
      //1.414 é a raiz quadrada de 2 arredondada
      //serve para aumentar a relevância do exploration no cálculo do UCT 
    return exploitation + 1.2 * exploration;
}

// faz o cálculo do UCT para todos os filhos e escolhe o melhor deles
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

// função para expandir um node (criar os seus filhos, ou seja, criar nodes para todas as jogadas possíveis)
// esta função também já coloca o node atual na board de "jogo falso"
Node *expand(Node *node)
{
    load_theory_board(node->board);
    
    char theory_player = node->current_player;

    Move moves[32];

    int move_count = generate_moves(moves);

    for(int i=0;i<move_count;i++)
    {
        load_theory_board(node->board);

        theory_player = node->current_player;

        Move m = moves[i];

        apply_move(m);

        Node *child = create_node(node,
                                  m,
                                  theory_board,
                                  theory_player);

        node->children[node->child_count++] = child;
    }

    node->fully_expanded = true;

    if(node->child_count == 0)
    {
        return node;
    }

    return node->children[rand()%node->child_count];
}

// execução do jogo falso do MonteCarlo
double rollout(Node *node)
{
    load_theory_board(node->board);

    theory_player = node->current_player;

    for(int depth=0; depth<1000; depth++)
    {
        Move moves[32];

        int count = generate_moves(moves);
        // 0.5 é usado como um valor-erro (já que não é inteiro)
        if(count == 0)
        {
            return 0.5;
        }
        
        if (theory_pop_win_con(0, 0) || theory_insert_win_con(0, 0)){
        return 1.0;
       }
        Move m = moves[rand()%count];

        int result = apply_move(m);
       
        //valor ao ganhar irá ser adicionado ao node ao dar return
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

void theory_player_switch(){
  if(theory_player == '#') { theory_player = '@';}
  else {theory_player = '#';}
}

// função para adicionar a todos os nodes anteriores na árvore o resultado do jogo do rollout
void backpropagate(Node *node, double result)
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

// função para libertar a memória da árvore do MCTS 
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

// junção de todo o código executado numa jogada pelo MonteCarlo 
Move monte_carlo_move(int iterations)
{
    Move root_move = {0,0};

    Node *root = create_node(NULL, root_move, board, player);

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

// informação para o usuário sobre o que é que o AI fez 
void ai_turn(int iterations)
{
    Move best = monte_carlo_move(iterations);

    printf("AI chose: column %d | %s", best.pos, best.option == 0 ? "insert\n" : "pop\n");

    game_option_selector(best.pos, best.option);
    player_switch();

}

