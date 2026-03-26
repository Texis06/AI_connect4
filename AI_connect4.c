#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAX_Y 6
#define MAX_X 7

void initiate_board(char board[MAX_X][MAX_Y]);
void print_board(char board[MAX_X][MAX_Y]);
int player_input(char board[MAX_X][MAX_Y]);
bool check_insert(char board[MAX_X][MAX_Y], int pos);
void insert(char board[MAX_X][MAX_Y], char player, int pos);


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
int player_input(char board[MAX_X][MAX_Y])
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


//check if you can insert in pos
bool check_insert(char board[MAX_X][MAX_Y], int pos) {return board[pos][0]=='_'; }


//allows players to insert their respective piece in the board
void insert(char board[MAX_X][MAX_Y], char player, int pos)
{
    int y=0;
    while(board[pos][y] == '_' && y<6) {y++; }
    if(y!=0) {board[pos][y-1] = player; }
    else {board[pos][y] = player; }
}

//win condition



int main()
{
    char game[MAX_X][MAX_Y];
    initiate_board(game);
    print_board(game);
    int player;
    while(true)
    {
        player = player_input(game);
        insert(game,'#',player);
        print_board(game);
        player = player_input(game);
        insert(game,'*',player);
        print_board(game);
    }


    return 0;
}