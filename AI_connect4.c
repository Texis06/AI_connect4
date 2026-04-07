#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
TODO:
-diferentes empates(campo cheio = POSSIBILIDADE de empate // se o gamestate for repetido 3 vezes então há POSSIBILIDADE de empate);
-win condition de insert; X
-win condition de pop(jogador que faz o pop tem a prioridade na vitória);
-colocar as funções feitas no topo;
-pop; X
-seleção jogadas;
-alterar board para ser global; X
*/



#define MAX_Y 6
#define MAX_X 7

bool true_board[MAX_X][MAX_Y];
char board[MAX_X][MAX_Y];
bool victory=false;
bool board_filled=false;



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
bool check_pop(int pos);
void pop(int pos, char player);
void print_ruleset();
void option_selector(char player);
char pop_win_con(int posx, int posy, char player);



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


/*
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
*/


//checks if its possible to insert, basicamente se o topo está todo cheio
bool check_top_insert(int i)
{
    if(i>=MAX_Y) {return false; }
    if(board[MAX_X-1][i] == '_') {return true; }
    else {return check_top_insert(i++); }
} 

void print_ruleset()
{
    printf("\n\n==========================================================================================");
    printf("\nrules for PopOut:\n");
    printf("-if you wish the choose a different position after selecting, choose 0 for the option;\n");
    printf("-if there is only one option available choose any number besides 0 to confirm the play\n");
    printf("-if the board is completely filled with pieces, you will have an option to tie the game;\n");
    printf("\n==========================================================================================\n\n");
}

//selects the option you want to choose
void option_selector(char player)
{
    int pos=0, option=0;
    while(pos==0)
    {
        printf("player %c, choose which position(1-7) you want to target:", player);
        while(true)
        {
            scanf("%d", &pos);
            if(pos<7 && pos>0) {break; }
            else {printf("\nThe position must be between 1-7"); }
        }
        if(check_insert(pos-1) && !check_pop(pos-1))
        {
            printf("choose if you want to insert(1) or pop(2):");
            scanf("%d", &option);
            if(option==1) {insert(player,pos); }
            else if(option==2) {pop(pos, player); }
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
/*    while(true)
    {
        scanf("%d",&input);
        if(input<-1 || input>7) {printf("the number must be between 1-7\n"); }
        else if(!check_insert(input-1)) {printf("chose somewhere thats not occupied\n"); }
        else {break; }
    }*/


//check if you can insert in pos
bool check_insert(int pos) {return board[pos][0]=='_'; }
//check if you can pop in pos
bool check_pop(int pos) {return board[pos][MAX_Y-1]=='_'; }


//allows players to insert their respective piece in the board
void insert(char player, int pos)
{
    int y=0;
    while(board[pos-1][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos-1][y-1] = player; }
    else {board[pos-1][y] = player; }
    if(insert_win_con(player, pos-1, y-1))
    {
        printf("\n================");
        printf("\nplayer %c victory\n", player);
        printf("================\n");
        victory = true;
    }
}

char pop_win_con(int posx, int posy, char player)
{
    if(horizontal_win_con(posx, posy, player) >= 4) {return player; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, -1) >= 4) {return player; }
    reset_true_board();
    if(diagonal_win_con(posx, posy, player, 1) >= 4) {return player; }
    reset_true_board();
    if(player=='#') {player='*'; }
    else {player='#'; }
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
void pop(int pos,char player)
{
    board[pos-1][MAX_Y-1] = '_';
    int i = MAX_Y-1;
    while(true)
    {
        if(i-1 < 0 || board[MAX_X][i-1] == '_') {break; }
        board[pos-1][i] = board[pos-1][i-1];
        if(!victory && pop_win_con(pos-1, i, player) != '_')
        {
            printf("\n================");
            printf("\nplayer %c victory\n", pop_win_con(pos-1, i, player));
            printf("================\n");
            victory = true;
        }
        i--;
    }
    board[pos-1][i] = '_';
}



int main()
{
    initiate_board();
    print_ruleset();
    print_board();
    reset_true_board();
    //int pos;
    char player='#';
    while(!victory)
    {
        //pos = player_input_insert();
        option_selector(player);
        print_board();
        if(player == '#') {player = '*'; }
        else {player = '#'; }
    }

    return 0;
}

