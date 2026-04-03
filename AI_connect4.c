#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
TODO:
-diferentes empates(campo cheio = POSSIBILIDADE de empate // se o gamestate for repetido 3 vezes então há POSSIBILIDADE de empate);
-win condition de insert;
-win condition de pop(jogador que faz o pop tem a prioridade na vitória);
-colocar as funções feitas no topo;
-pop;
-seleção jogadas;
-alterar board para ser global;
*/



#define MAX_Y 6
#define MAX_X 7

bool true_board[MAX_X][MAX_Y];
char board[MAX_X][MAX_Y];
bool victory=false;

void initiate_board();
void print_board();
int player_input_insert();
bool check_insert(int pos);
void insert(char player, int pos);
bool insert_win_con(char player, int posx, int posy);
int horizontal_win_con(int x, int y, char player);
int vertical_win_con(int x, int y, char player);
int diagonal_win_con(int x, int y, char player, int dir);
void reset_true_board();
bool check_top_insert(int i);
void option_selector();
bool check_pop(int pos);


//creates/resets the board state
void initiate_board()
{
    for(int y=0;y<MAX_Y;y++)
    {
        for(int x=0;x<MAX_X;x++) {board[x][y] = '_'; }
    }
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



//allows player to choose which coordinate to play
int player_input_insert()
{
    int input;
    while(true)
    {
        scanf("%d",&input);
        if(input<-1 || input>7) {printf("the number must be between 1-7\n"); }
        else if(!check_insert(input-1)) {printf("chose somewhere thats not occupied\n"); }
        else {break; }
    }
    return input-1;
}

//checks if its possible to insert, basicamente se o topo está todo cheio
bool check_top_insert(int i)
{
    if(i>=MAX_Y) {return false; }
    if(board[MAX_X-1][i] == '_') {return true; }
    else {return check_top_insert(i++); }
} 

//selects the option you want to choose
void option_selector()
{
    int option;
    printf("choose if you want to pop out (1) or insert (2), write number of your option:");
    scanf("%d", &option);
}




//check if you can insert in pos
bool check_insert(int pos) {return board[pos][0]=='_'; }
//check if you can pop in pos
bool check_pop(int pos) {return board[pos][MAX_Y-1]=='_'; }


//allows players to insert their respective piece in the board
void insert(char player, int pos)
{
    int y=0;
    while(board[pos][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos][y-1] = player; }
    else {board[pos][y] = player; }
    if(insert_win_con(player, pos, y-1))
    {
        printf("\n================");
        printf("\nplayer %c victory\n", player);
        printf("================\n");
        victory = true;
    }
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
void pop(int pos)
{
    board[pos][MAX_Y-1] = '_';
    int i = MAX_Y-1;
    while(true)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        board[pos][i] = board[pos][i-1];
        i--;
    }
    board[pos][i] = '_';
}



int main()
{
    initiate_board();
    print_board();
    reset_true_board();
    int pos;
    char player='#';
    while(!victory)
    {
        pos = player_input_insert();
        insert(player,pos);
        print_board();
        if(player == '#') {player = '*'; }
        else {player = '#'; }
    }
    scanf("%d", &pos);
    pop(pos-1);
    print_board();


    return 0;
}

