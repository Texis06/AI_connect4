#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAX_Y 6
#define MAX_X 7


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
}


//allows player to choose which coordinate to play
int player_input(char board[MAX_X][MAX_Y])
{
    int input;
    do
    {
        scanf("%d",&input);
        if(input<0 || input >8) {printf("the number must be between 1-7"); }
    } while (input<0 || input >8);
    return input;
}


//check if you can insert in pos
bool checks_insert(char board[MAX_X][MAX_Y], int pos) {return board[pos][0]!='_'; }


//allows players to insert their respective piece in the board
void insert(char board[MAX_X][MAX_Y], char player, int pos)
{
    int y=0;
    while(board[pos][y+1] == '_' || y == MAX_Y) {y++; }
    board[y][pos] = player;
}



int main()
{
    char game[MAX_X][MAX_Y];
    initiate_board(game);
    print_board(game);
    while(true)
    {
        insert(game, '#', 2);
        print_board(game);
    }


    return 0;
}