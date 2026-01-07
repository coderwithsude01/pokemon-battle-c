#include <stdio.h>
#include "../include/prototypes.h"

int main(void)
{
    Type Types[18];
    Move Moves[486];
    Pokemon Pokemons[1015];

    Player p1;
    Player  p2;

    initialize(Types, Moves, Pokemons, &p1, &p2);
    game(&p1, &p2, Types);

    return 0;


}