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
*/



#define MAX_Y 6
#define MAX_X 7

void initiate_board(char board[MAX_X][MAX_Y]);
void print_board(char board[MAX_X][MAX_Y]);
int player_input_insert(char board[MAX_X][MAX_Y]);
bool check_insert(char board[MAX_X][MAX_Y], int pos);
void insert(char board[MAX_X][MAX_Y], char player, int pos);
bool insert_win_con(char board[MAX_X][MAX_Y], char player, int posx, int posy);
int horizontal_win_con(int x, int y, char board[MAX_X][MAX_Y], char player);
int vertical_win_con(int x, int y, char board[MAX_X][MAX_Y], char player);
int diagonal_win_con(int x, int y, char board[MAX_X][MAX_Y], char player, int dir);



//creates/resets the board state
void initiate_board(char board[MAX_X][MAX_Y])
{
    for(int y=0;y<MAX_Y;y++)
    {
        for(int x=0;x<MAX_X;x++) {board[x][y] = '_'; }
    }
}


//prints the board
void print_board(char board[MAX_X][MAX_Y])
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
    for(int i=0;i<MAX_X+2;i++) {printf("-"); }
    printf("\n\n");
}



//allows player to choose which coordinate to play
int player_input_insert(char board[MAX_X][MAX_Y])
{
    int input;
    while(true)
    {
        scanf("%d",&input);
        if(input<-1 || input>7) {printf("the number must be between 1-7\n"); }
        else if(!check_insert(board, input-1)) {printf("chose somewhere thats not occupied\n"); }
        else {break; }
    }
    return input-1;
}

//checks if its possible to insert, basicamente se o topo está todo cheio
bool check_top_insert(int i,char board[MAX_X][MAX_Y])
{
    if(i>=MAX_Y) {return false; }
    if(board[MAX_X-1][i] == '_') {return true; }
    else {return check_top_insert(i++, board); }
} 

//selects the option you want to choose
void option_selector(char board[MAX_X][MAX_Y])
{
    int option;
    printf("choose if you want to pop out (1) or insert (2), write number of your option:");
    scanf("%d", &option);
    if(!check_top_insert(0, board))
    {
    //    printf("");
    }
    
}


//check if you can insert in pos
bool check_insert(char board[MAX_X][MAX_Y], int pos) {return board[pos][0]=='_'; }
//check if you can pop in pos
bool check_pop(char board[MAX_X][MAX_Y], int pos) {return board[pos][MAX_Y-1]=='_'; }


//allows players to insert their respective piece in the board
void insert(char board[MAX_X][MAX_Y], char player, int pos)
{
    int y=0;
    while(board[pos][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos][y-1] = player; }
    else {board[pos][y] = player; }
    /*
    if(insert_win_con(board, player, pos, y))
    {
        printf("\nvitória\n");
    }*/
}


//win condition
bool insert_win_con(char board[MAX_X][MAX_Y], char player, int posx, int posy)
{
    if(horizontal_win_con(posx, posy, board, player)>=4 || vertical_win_con(posx, posy, board, player)>=4 || diagonal_win_con(posx, posy, board, player, -1)>=4 || diagonal_win_con(posx, posy, board, player, 1)>=4)
    {return true; }
    else {return false; }
}

int horizontal_win_con(int x, int y, char board[MAX_X][MAX_Y], char player)
{
    printf("%d %d\n", x, y);
    if(x>=MAX_X || x<=-1) {return 0; }
    if(board[x][y]!=player) {return 0; }
    return 1 + horizontal_win_con(x-1, y, board, player) + horizontal_win_con(x+1, y, board, player);
}

int vertical_win_con(int x, int y, char board[MAX_X][MAX_Y], char player)
{
    if(y>=MAX_Y || y<=-1) {return 0; }
    if(board[x][y]!=player) {return 0; }
    return 1 + vertical_win_con(x, y-1, board, player) + vertical_win_con(x, y+1, board, player);
}

int diagonal_win_con(int x, int y, char board[MAX_X][MAX_Y], char player, int dir)
{
    if(y>=MAX_Y || y<=-1 || x>=MAX_X || x<=-1 ) {return 0; }
    if(board[x][y]!=player) {return 0; }
    return 1 + diagonal_win_con(x+1, y-dir, board, player, dir) + diagonal_win_con(x-1, y+dir, board, player, dir);
}


//option chooser



//pop




int main()
{
    char game[MAX_X][MAX_Y];
    initiate_board(game);
    print_board(game);
    int player;
    while(true)
    {
        player = player_input_insert(game);
        insert(game,'#',player);
        print_board(game);
        printf("%d", horizontal_win_con(0,MAX_Y-1,game,'#'));
        printf("%d", vertical_win_con(0,MAX_Y-1,game,'#'));
        printf("%d", diagonal_win_con(0,MAX_Y-1,game,'#',1));
        printf("%d\n", diagonal_win_con(0,MAX_Y-1,game,'#',-1));
    }


    return 0;
}

