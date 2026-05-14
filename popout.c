#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
/*
TODO:
-criar sistema para empate por repetição para o bot;
-sistema separado para o bot não acabar com o jogo nas simulações;
-smth smth idk tou cansado;
*/


#define PRIME 3323
#define MAX_Y 6
#define MAX_X 7

char player = '#';
char board[MAX_X][MAX_Y];
char theory_board[MAX_X][MAX_Y];
bool true_board[MAX_X][MAX_Y];
bool victory=false;
bool board_filled=false;
bool state_repeated=false;

bool ai_options[8][2];
/*
ai_options:
0,0 - empate
x,0 - insert
x,1 - pop
*/

uint64_t bucket[PRIME];
unsigned short int bucket_count[PRIME];

typedef struct Key84
{
    uint64_t lo;
    uint32_t hi;
} Key84;



//internal
Key84 hash_parser();
uint64_t hash84(Key84 k);
void bucket_sender();
bool check_filled_board(int x);
bool check_insert(int pos);
bool check_pop(int pos);
void insert(int pos);
void pop(int pos);
bool insert_win_con(int posx, int posy);
bool pop_win_con(int posx, int posy);
int horizontal_win_con(int x, int y);
int vertical_win_con(int x, int y);
int diagonal_win_con(int x, int y, int dir);
void reset_true_board();
void player_switch();
bool ai_option_shower_helper(int x);

//para serem usadas
void human_option_selector();
void init_theory_board();
void print_ruleset();
void print_board();
void initiate_board();
void ai_option_selector(int pos, int option);


Key84 hash_parser()
{
    Key84 x84 = {0,0};
    char *ptr = &board[0][0];
    
    for(int i=0;i<MAX_X*MAX_Y;i++)
    {
        uint8_t v=0;
    
        switch(*(ptr + i))
        {
            case '@':
            v |= 1;
            break;

            case '#':
            v |= 2;
            break;

            case '_':
            break;
        }
        uint32_t carry = (x84.lo >> 62) & 0x3;
        // shift both halves
        x84.lo <<= 2;
        x84.hi <<= 2;
        // move overflow bits into hi
        x84.hi |= carry;
        // insert new symbol
        x84.lo |= v;
    }
    return x84;
}

uint64_t hash84(Key84 k)
{
    uint64_t x = k.lo;

    x ^= ((uint64_t)k.hi << 32);

    // splitmix64 finalizer
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebULL;
    x ^= x >> 31;

    return x;
}

void bucket_sender()
{
    Key84 x86 = hash_parser();
    uint64_t hashed = hash84(x86);
    bucket [hashed % PRIME] = hashed;
    if(bucket_count[hashed % PRIME] == 0) {bucket_count[hashed % PRIME]=1; }
    else if(bucket_count[hashed % PRIME] >= 2) {state_repeated=true; }
    else {bucket_count[hashed % PRIME]++; }
    //printf("\n%hu\n", bucket_count[hashed % PRIME]);
}


void init_theory_board()
{
    for(int x=0;x<MAX_X;x++)
    {
        for(int y=0;y<MAX_Y;y++) {theory_board[x][y] = board[x][y]; }
    }
}


//creates/resets the board state
void initiate_board()
{
    for(int y=0;y<MAX_Y;y++)
    {
        for(int x=0;x<MAX_X;x++) {board[x][y] = '_'; }
    }
}

void player_switch()
{
    if(player == '#') {player='@'; }
    else {player='#'; }
}


void reset_true_board()
{
    for(int y=0;y<MAX_Y;y++)
    {
        for(int x=0;x<MAX_X;x++) {true_board[x][y] = false; }
    }
}

//prints the board
void print_board()
{
    for(int y=0;y<MAX_Y;y++)
    {
        printf("|");
        for(int x=0;x<MAX_X;x++) 
        {
            printf("%c", board[x][y]);
        }
        printf("|\n");
    }
    printf("-");
    for(int i=1;i<=MAX_X;i++) {printf("%d", i); }
    printf("-");
    printf("\n\n");
}

//regras
void print_ruleset()
{
    printf("\n\n================================================================================================");
    printf("\nrules for PopOut:\n");
    printf("-if you wish the choose a different position after selecting, choose 0 for the option;\n");
    printf("-if there is only one option available choose any number besides 0 to confirm the play\n");
    printf("-if the board is completely filled with pieces, you will have the option to tie the game;\n");
    printf("-if the same position is repeated more that 3 times both players will have the option to draw\n");
    printf("================================================================================================\n\n");
}

//opções para humano selecionar, precisa de um loop externo para ser repetido
void human_option_selector()
{
    init_theory_board();
    bucket_sender();
    int pos=0, option=0;
    while(option==0)
    {
        if(state_repeated)
        {
            printf("the same moves have been repeated 3 or more times, either player can tie(1) or continue(2)\n");
            printf("player %c:", player);
            scanf("%d", &option);
            if(option!=1)
            {
                player_switch();
                printf("\nplayer %c:", player);
                player_switch();
                scanf("%d", &option);
            }
            if(option==1)
            {
                victory=true;
                printf("\n================");
                printf(" the game ended with a tie ");
                printf("================\n");
                break;
            }
            state_repeated=false;
        }
        if(board_filled)
        {
            printf("the board is currently filled, you can tie the game(1) or continue(2)\n");
            scanf("%d", &option);
            if(option==1)
            {
                victory=true;
                printf("\n================");
                printf(" the game ended with a tie ");
                printf("================\n");
                break;
            }
            board_filled=false;
        }
        printf("player %c, choose which position(1-7) you want to target:", player);
        while(true)
        {
            scanf("%d", &pos);
            if(pos<8 && pos>0) {break; }
            else {printf("\nthe position must be between 1-7:"); }
        }
        if(check_insert(pos-1) && check_pop(pos-1))
        {
            printf("choose if you want to insert(1) or pop(2):");
            scanf("%d", &option);
            if(option==1) {insert(pos); break; }
            else if(option==2) {pop(pos); break; }
        }
        else if(check_insert(pos-1))
        {
            printf("you can only insert:");
            scanf("%d", &option);
            if(option!=0) {insert(pos); }
        }
        else if(check_pop(pos-1))
        {
            printf("you can only pop:");
            scanf("%d", &option);
            if(option!=0) {pop(pos); }
        }
        else {printf("you can't pop or insert, choose a different position\n"); }
    }
    player_switch();
}


//verifica se local para inserir está livre
bool check_insert(int pos) {return board[pos][0]=='_'; }
//verifica se local para pop está livre
bool check_pop(int pos) {return board[pos][MAX_Y-1]==player; }

bool check_filled_board(int x)
{
    if(x>=MAX_X) {return true; }
    if(board[x][0] == '_') {return false; }
    else return check_filled_board(x+1);
}
//verifica se a board está cheia
/*bool check_filled_board(int x)
{
    int i=0;
    while(i<MAX_X)
    {
        if(board[i][0]=='_') {return false; }
        i++;
    }
    board_filled=true;
    return true;
}*/

//todas as win_cons estão aqui
bool pop_win_con(int posx, int posy)
{
    if(horizontal_win_con(posx, posy) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, -1) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, 1) >= 4) {return true; }
    reset_true_board();
    return false;
}

bool insert_win_con(int posx, int posy)
{
    if(horizontal_win_con(posx, posy) >= 4) {return true; }
    reset_true_board();
    if(vertical_win_con(posx, posy) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, -1) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, 1) >= 4) {return true; }
    reset_true_board();
    return false;
}

int horizontal_win_con(int x, int y)
{
    if(x>=MAX_X || x<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + horizontal_win_con(x-1, y) + horizontal_win_con(x+1, y);
}

int vertical_win_con(int x, int y)
{
    if(y>=MAX_Y || y<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + vertical_win_con(x, y-1) + vertical_win_con(x, y+1);
}

int diagonal_win_con(int x, int y, int dir)
{
    if(y>=MAX_Y || y<=-1 || x>=MAX_X || x<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + diagonal_win_con(x+1, y-dir, dir) + diagonal_win_con(x-1, y+dir, dir);
}

////função para dar pop a pieces, também verifica vitóra
void pop(int pos)
{
    board[pos-1][MAX_Y-1] = '_';
    int i = MAX_Y-1;
    while(true)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        board[pos-1][i] = board[pos-1][i-1];
        i--;
    }
    board[pos-1][i] = '_';
    i = MAX_Y-1;
    while(true)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        if(!victory && pop_win_con(pos-1, i))
        {
            printf("\n=================");
            printf("\nplayer %c victory\n", player);
            printf("=================\n");
            victory = true;
            break;
        }
        i--;
    }
    player_switch();
    i = MAX_Y-1;
    while(!victory)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        if(!victory && pop_win_con(pos-1, i))
        {
            printf("\n=================");
            printf("\nplayer %c victory\n", player);
            printf("=================\n");
            victory = true;
            break;
        }
        i--;
    }
    player_switch();
}

//função para colocar pieces, também verifica vitóra
void insert(int pos)
{
    int y=0;
    while(board[pos-1][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos-1][y-1] = player; }
    else {board[pos-1][y] = player; }
    if(insert_win_con(pos-1, y-1))
    {
        printf("\n=================");
        printf("\nplayer %c victory\n", player);
        printf("=================\n");
        victory = true;
    }
    if(!check_insert(pos) && check_filled_board(0)) {board_filled=true; }
}
//ai_options[8][2]
void ai_option_shower()
{
    if(board_filled || state_repeated) {ai_options[0][0] = true; }
    ai_option_shower_helper(0);
}

bool ai_option_shower_helper(int x)
{
    if(x>=MAX_X) {return true; }
    if(board[x][MAX_Y-1] == player) {ai_options[x+1][1] = true; }
    else {ai_options[x+1][1] = false; }
    if(board[x][0] == '_') {ai_options[x+1][0] = true; }
    else {ai_options[x+1][0] = false; }
    return ai_option_shower_helper(x+1);
}

/*
ai_options:
0,0 - empate
x,0 - insert
x,1 - pop
*/

void ai_option_selector(int pos, int option)
{
    if(pos == 0 && option == 0) {victory=true; }
    else if(option == 0) {insert(pos); }
    else {pop(pos); }
    player_switch();
}




int main()
{
    initiate_board();
    print_ruleset();
    print_board();
    reset_true_board();
    ai_option_shower();
    while(!victory)
    {
        human_option_selector();
        print_board();
    }

    return 0;
}
