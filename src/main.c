#include "prototypes.h"                                                                               
                                                                                                       
 int main() {   
    
    Type Types[18];
    Move Moves[486];
    Pokemon Pokemons[1015];

    Player Player1;
    Player Player2;

    initialize(Types, Moves, Pokemons, &Player1, &Player2);
    game(&Player1, &Player2, Types);

    return 0;

        
  }                   