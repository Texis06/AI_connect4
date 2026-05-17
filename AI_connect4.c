#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
//#include "popout.c"
#include "MonteCarlo.c"


int main()
{
    initiate_board();
    int flag;
    printf("Choose your gamemode:\n-(1)player vs player;\n-(2)player vs AI;\n-(3)AI vs AI;\n");
    scanf("%d", &flag);
    switch(flag)
    {
        case 1:
        {
            print_ruleset();
            while(!victory)
            {
                print_board();
                human_option_selector();
            }
            break;
        }
        case 2:
        {
            print_ruleset();
            while(!victory)
            {
                print_board();
                human_option_selector();
                if(!victory)
                {
                    //player_switch();
                    ai_turn(5000);
                }
            }
            break;
        }
        case 3:
        {
            
            break;
        }


    }
    return 0;
}