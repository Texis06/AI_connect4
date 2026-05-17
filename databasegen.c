#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "MonteCarlo.c"

void board_to_string(char *out)
{
    int k = 0;
    for(int y = 0; y < MAX_Y; y++)
        for(int x = 0; x < MAX_X; x++)
            out[k++] = board[x][y];
    out[k] = '\0';
}

void run_game(FILE *fptr, int iterations)
{
    initiate_board();
    victory = false;
    player = '#';

    char board_str[MAX_X * MAX_Y + 1];

    while(!victory)
    {
        board_to_string(board_str);

        Move best = monte_carlo_move(iterations);

        char move_type;
        if(best.pos == 0)       move_type = 'd'; // draw
        else if(best.option == 0) move_type = 'i'; // insert
        else                      move_type = 'p'; // pop


         if(best.pos == 0)
          fprintf(fptr, "draw,%s,%c\n", board_str, player);
         else
          fprintf(fptr, "%d-%c,%s,%c\n", best.pos, move_type, board_str, player);
        game_option_selector(best.pos, best.option);
        player_switch();
    }
}

int main()
{
    srand(time(NULL));

    int num_games, iterations;
    printf("How many games? ");
    scanf("%d", &num_games);
    printf("Iterations per move? ");
    scanf("%d", &iterations);

    FILE *fptr = fopen("dataset.csv", "w");
    fprintf(fptr, "move,board_state,player\n");

    for(int i = 0; i < num_games; i++)
    {
        printf("Game %d/%d\n", i+1, num_games);
        run_game(fptr, iterations);
    }

    fclose(fptr);
    printf("Done! Saved to dataset.csv\n");
    return 0;
}

