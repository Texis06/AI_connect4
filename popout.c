#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
/*
TODO:
-diferenciar funções entre jogadores e AI;
-criar funções para papar fruta de robô; 
*/


#define PRIME 2557
#define MAX_Y 6
#define MAX_X 7

bool true_board[MAX_X][MAX_Y];
char board[MAX_X][MAX_Y];
char prev_board[MAX_X][MAX_Y];
bool victory=false;
bool board_filled=false;
bool state_repeated=false;
uint64_t bucket[PRIME];
unsigned short int bucket_count[PRIME];

typedef struct Key84
{
    uint64_t lo;
    uint32_t hi;
} Key84;

void initiate_board();
void print_board();
bool check_insert(int pos);
void insert(char player, int pos);
bool insert_win_con(char player, int posx, int posy);
char pop_win_con(int posx, int posy, char player);
int horizontal_win_con(int x, int y, char player);
int vertical_win_con(int x, int y, char player);
int diagonal_win_con(int x, int y, char player, int dir);
void reset_true_board();
bool check_top_insert(int y);
bool check_pop(int pos, char player);
void pop(int pos, char player);
void print_ruleset();
void option_selector(char player);
void player_switch(char *player);
Key84 hash_parser();
uint64_t hash84(Key84 k);
void prev_board_state();
void bucket_sender();
bool check_filled_board();




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
    printf("\n%hu\n", bucket_count[hashed % PRIME]);
}


void prev_board_state()
{
    for(int x=0;x<MAX_X;x++)
    {
        for(int y=0;y<MAX_Y;y++) {prev_board[x][y] = board[x][y]; }
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

void player_switch(char *player)
{
    if(*player == '#') {*player='@'; }
    else {*player='#'; }
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


//checks if its possible to insert, basicamente se o topo está todo cheio
bool check_top_insert(int y)
{
    if(y>=MAX_Y) {return false; }
    if(board[MAX_X-1][y] == '_') {return true; }
    else {return check_top_insert(y++); }
} 

void print_ruleset()
{
    printf("\n\n==========================================================================================");
    printf("\nrules for PopOut:\n");
    printf("-if you wish the choose a different position after selecting, choose 0 for the option;\n");
    printf("-if there is only one option available choose any number besides 0 to confirm the play\n");
    printf("-if the board is completely filled with pieces, you will have the option to tie the game;\n");
    printf("-if the same position is used for the same\n"); //escrever ts (this shit)
    printf("==========================================================================================\n\n");
}

//selects the option you want to choose
void option_selector(char player)
{
    prev_board_state();
    bucket_sender();
    int pos=0, option=0;
    while(pos==0)
    {
        if(state_repeated)
        {
            printf("the same moves have been repeated 3 or more times, either player can tie(1) or continue(2)\n");
            printf("player %c:", player);
            scanf("%d", &option);
            if(option!=1)
            {
                player_switch(&player);
                printf("\nplayer %c:", player);
                player_switch(&player);
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
        }
        printf("player %c, choose which position(1-7) you want to target:", player);
        while(true)
        {
            scanf("%d", &pos);
            if(pos<8 && pos>0) {break; }
            else {printf("\nthe position must be between 1-7:"); }
        }
        if(check_insert(pos-1) && check_pop(pos-1, player))
        {
            while(true)
            {
                printf("choose if you want to insert(1) or pop(2):");
                scanf("%d", &option);
                if(option==1) {insert(player, pos); break; }
                else if(option==2) {pop(pos, player); break; }
            }
        }
        else if(check_insert(pos-1))
        {
            printf("you can only insert:");
            scanf("%d", &option);
            if(option!=0) {insert(player, pos); }
        }
        else
        {
            printf("you can only pop:");
            scanf("%d", &option);
            if(option!=0) {pop(pos, player); }
        }
    }

}


//check if you can insert in pos
bool check_insert(int pos) {return board[pos][0]=='_'; }
//check if you can pop in pos
bool check_pop(int pos, char player) {return board[pos][MAX_Y-1]==player; }
//checks if the board is full
bool check_filled_board()
{
    int i=0;
    while(i<MAX_X)
    {
        if(board[i][0]=='_') {return false; }
        i++;
    }
    board_filled=true;
    return true;
}


//allows players to insert their respective piece in the board
void insert(char player, int pos)
{
    int y=0;
    while(board[pos-1][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos-1][y-1] = player; }
    else {board[pos-1][y] = player; }
    if(insert_win_con(player, pos-1, y-1))
    {
        printf("\n=================");
        printf("\nplayer %c victory\n", player);
        printf("=================\n");
        victory = true;
    }
    if(!check_insert(pos) && check_filled_board()) {; }
}


char pop_win_con(int posx, int posy, char player)
{
    if(horizontal_win_con(posx, posy, player) >= 4) {return player; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, -1) >= 4) {return player; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, 1) >= 4) {return player; }
    reset_true_board();
    return '_';
}

//win condition
bool insert_win_con(char player, int posx, int posy)
{
    if(horizontal_win_con(posx, posy, player) >= 4) {return true; }
    reset_true_board();
    if(vertical_win_con(posx, posy, player) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, -1) >= 4) {return true; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, 1) >= 4) {return true; }
    reset_true_board();
    return false;
}

int horizontal_win_con(int x, int y, char player)
{
    if(x>=MAX_X || x<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + horizontal_win_con(x-1, y, player) + horizontal_win_con(x+1, y, player);
}

int vertical_win_con(int x, int y, char player)
{
    if(y>=MAX_Y || y<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + vertical_win_con(x, y-1, player) + vertical_win_con(x, y+1, player);
}

int diagonal_win_con(int x, int y, char player, int dir)
{
    if(y>=MAX_Y || y<=-1 || x>=MAX_X || x<=-1 || true_board[x][y] == true) {return 0; }
    true_board[x][y] = true;
    if(board[x][y]!=player) {return 0; }
    return 1 + diagonal_win_con(x+1, y-dir, player, dir) + diagonal_win_con(x-1, y+dir, player, dir);
}

//pop
void pop(int pos, char player)
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
        if(!victory && pop_win_con(pos-1, i, player) != '_')
        {
            printf("\n=================");
            printf("\nplayer %c victory\n", player);
            printf("=================\n");
            victory = true;
            break;
        }
        i--;
    }
    player_switch(&player);
    i = MAX_Y-1;
    while(!victory)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        if(!victory && pop_win_con(pos-1, i, player) != '_')
        {
            printf("\n=================");
            printf("\nplayer %c victory\n", player);
            printf("=================\n");
            victory = true;
            break;
        }
        i--;
    }
}

/*
int main()
{
    initiate_board();
    print_ruleset();
    print_board();
    reset_true_board();
    char player='#';
    while(!victory)
    {
        option_selector(player);
        print_board();
        player_switch(&player);
    }

    return 0;
}
*/
